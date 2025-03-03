import json
import os
import shutil
from django.core.paginator import Paginator
from django.db.models import Q
from django.forms import model_to_dict
from django.http import JsonResponse, HttpResponseNotAllowed
from .errorcode import ErrorCode
import logging
from .models import House
from .utils import *
from django.utils import timezone
from datetime import datetime
from .pathutil import PathUtil


logger = logging.getLogger('house')

g_image_path = PathUtil.get_image_path()
g_cache_path = PathUtil.get_cache_path()

g_image_keys = ['fangchanzheng_images', 'shuipiao_images', 'tongzhishu_images',
                'buyer_hukou_images', 'buyer_id_images', 'buyer_marriage_images',
                'seller_hukou_images', 'seller_id_images', 'seller_marriage_images',
                'wangqianhetong_images']


# 从提交的参数body获取house的数据，成功返回(data, None)，失败返回(None, response)
def get_house_data(request_body):
    valid_keys = [field.name for field in House._meta.get_fields()]
    data_dict = {key: value for key, value in request_body.items() if key in valid_keys}

    # 交易日期将utc时间戳改成date对象
    if 'jiaoyi_at' in data_dict:
        data_dict['jiaoyi_at'] = timezone.make_aware(datetime.fromtimestamp(int(data_dict['jiaoyi_at'])))

    # 校验所有图片是否存在，不存在就从cache移过去
    for image_key in g_image_keys:
        if image_key in data_dict:
            for image_id in data_dict[image_key]:
                image_file_path = os.path.join(g_image_path, image_id)
                if not os.path.exists(image_file_path):
                    cache_file_path = os.path.join(g_cache_path, image_id)
                    try:
                        shutil.move(cache_file_path, image_file_path)
                    except Exception as e:
                        logger.error('failed to move image file: {}, error: {}'.format(image_id, e))
                        return None, JsonResponse(make_response_body(code=ErrorCode.IMAGE_NOT_EXIST,
                                                  msg=ErrorCode.IMAGE_NOT_EXIST_MSG))

    # 将图片的参数内容改成字符串
    for image_key in g_image_keys:
        if image_key in data_dict:
            data_dict[image_key] = str(data_dict[image_key]).replace("'", '"')

    return data_dict, None


def create_house(request):
    try:
        request_body = json.loads(request.body)
        data_dict, response = get_house_data(request_body)
        if data_dict is None:
            return response

        # 创建house对象
        house = House(**data_dict)
        house.save()

        # 返回房子ID
        data = {'house_id': house.house_id}
        response_body = make_response_body(data=data)
        return JsonResponse(response_body)
    except Exception as e:
        logger.error("creating house have an error: {}".format(e))
        body = make_response_body(code=ErrorCode.WRONG_PARAM, msg=ErrorCode.WRONG_PARAM_MSG)
        return JsonResponse(body)


def handle_house(request, house_id):
    if request.method == 'DELETE':
        return delete_house(request, house_id)

    if request.method == 'PUT':
        return update_house(request, house_id)

    return HttpResponseNotAllowed(permitted_methods=['PUT', 'DELETE'])


def delete_house(_, house_id):
    try:
        house = House.objects.get(house_id=house_id)
        house.delete()
        return JsonResponse(make_response_body())
    except Exception as e:
        logger.error("deleting house have an error: {}".format(e))
        body = make_response_body(code=ErrorCode.WRONG_PARAM, msg=ErrorCode.WRONG_PARAM_MSG)
        return JsonResponse(body)


def update_house(request, house_id):
    try:
        request_body = json.loads(request.body)
        data_dict, response = get_house_data(request_body)
        if data_dict is None:
            return response

        house = House.objects.filter(house_id=house_id)
        if not house.exists():
            body = make_response_body(code=ErrorCode.HOUSE_NOT_EXIST, msg=ErrorCode.HOUSE_NOT_EXIST_MSG)
            return JsonResponse(body)

        house.update(**data_dict)
        return JsonResponse(make_response_body())
    except Exception as e:
        logger.error("updating house have an error: {}".format(e))
        body = make_response_body(code=ErrorCode.WRONG_PARAM, msg=ErrorCode.WRONG_PARAM_MSG)
        return JsonResponse(body)


def list_house(request):
    try:
        page = int(request.GET.get('page'))
        page_size = int(request.GET.get('page_size'))
        if page is None or page_size is None or page_size > 300 or page_size <= 0 or page <= 0:
            return JsonResponse(make_response_body(code=ErrorCode.WRONG_PARAM, msg=ErrorCode.WRONG_PARAM_MSG))

        # 获取查询条件
        # 支持房子名称模糊查询
        # 支持姓名模糊查询，只要买方/卖方/中介人/缴税人/承办人含有，就认为满足条件
        # 支持身份证号码模糊查询，只要买方/卖方含有，就认为满足条件
        # 姓名和身份证号码字段通过buyer_name和buyer_id传入
        query_conditions = []
        if 'house_name' in request.GET and len(request.GET.get('house_name')) > 0:
            house_name_condition = Q(house_name__icontains=request.GET.get('house_name'))
            query_conditions.append(house_name_condition)

        if 'buyer_name' in request.GET and len(request.GET.get('buyer_name')) > 0:
            name = request.GET.get('buyer_name')
            name_condition = (Q(jiaoshui_name__icontains=name) | Q(zhongjie_name__icontains=name) |
                              Q(buyer_name__icontains=name) | Q(seller_name__icontains=name) |
                              Q(chengban_name__icontains=name))
            query_conditions.append(name_condition)

        if 'buyer_id' in request.GET and len(request.GET.get('buyer_id')) > 0:
            shenfenzheng = request.GET.get('buyer_id')
            id_condition = Q(buyer_id__icontains=shenfenzheng) | Q(seller_id__icontains=shenfenzheng)
            query_conditions.append(id_condition)

        # 分页查询数据
        if len(query_conditions) == 0:
            houses = House.objects.filter().order_by('house_id')
        else:
            condition = query_conditions[0]
            for i in range(1, len(query_conditions)):
                condition = (condition & query_conditions[i])
            houses = House.objects.filter(condition).order_by('house_id')
        total_count = houses.count()
        page_houses = Paginator(houses, page_size).page(page)

        # 构造响应数据
        data_list = []
        for house in page_houses:
            house_obj = model_to_dict(house)
            # 将图片字符串转成数组
            for image_key in g_image_keys:
                if len(house_obj[image_key]) > 0:
                    house_obj[image_key] = json.loads(house_obj[image_key])
                else:
                    house_obj[image_key] = []
            # 交易日期转换为utc时间戳
            house_obj['jiaoyi_at'] = int(house.jiaoyi_at.timestamp())
            data_list.append(house_obj)
        response_body = make_response_body(data=make_response_data_list(total_count, data_list))
        return JsonResponse(response_body)
    except Exception as e:
        logger.error("listing house have an error: {}".format(e))
        body = make_response_body(code=ErrorCode.WRONG_PARAM, msg=ErrorCode.WRONG_PARAM_MSG)
        return JsonResponse(body)
