import logging
import os.path
import uuid

from django.http import JsonResponse, HttpResponseServerError, FileResponse
from .errorcode import ErrorCode
from .pathutil import PathUtil
from .utils import *

logger = logging.getLogger('image')


g_image_path = PathUtil.get_image_path()
g_cache_path = PathUtil.get_cache_path()


# 上传图片，保存到缓存目录下
def create_image(request):
    try:
        uploaded_file = request.FILES['file']
        image_id = str(uuid.uuid4())
        image_file_path = os.path.join(g_cache_path, image_id)
        with open(image_file_path, 'wb') as file:
            for chunk in uploaded_file.chunks():
                file.write(chunk)

        response_data = {
            'image_id': image_id
        }
        return JsonResponse(make_response_body(data=response_data))
    except Exception as e:
        logger.error("creating image has error: {}".format(e))
        return HttpResponseServerError("Internal Server Error. Please try again later.")


def get_image(_, image_id):
    try:
        file_path = os.path.join(g_image_path, image_id)
        if not os.path.exists(file_path):
            logger.error("the image of {} not found".format(image_id))
            return JsonResponse(make_response_body(code=ErrorCode.IMAGE_NOT_EXIST,
                                                   msg=ErrorCode.IMAGE_NOT_EXIST_MSG))

        return FileResponse(open(file_path, 'rb'))
    except Exception as e:
        logger.error("downloading image has error: {}".format(e))
        return HttpResponseServerError("Internal Server Error. Please try again later.")
