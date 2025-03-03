from django.db import models


# 房子表
class House(models.Model):
    # id
    house_id = models.AutoField(primary_key=True)

    # 房子名称
    house_name = models.CharField(max_length=100, db_index=True)

    # 缴税人
    jiaoshui_name = models.CharField(max_length=20, db_index=True)

    # 费用
    fee = models.CharField(max_length=20)

    # 中介人名字
    zhongjie_name = models.CharField(max_length=20, db_index=True)

    # 交易日期
    jiaoyi_at = models.DateTimeField(db_index=True)

    # 房产证图片，图片ID数组
    fangchanzheng_images = models.CharField(max_length=1000)

    # 税票图片，图片ID数组
    shuipiao_images = models.CharField(max_length=1000)

    # 受理通知书图片，图片ID数组
    tongzhishu_images = models.CharField(max_length=1000)

    # 买方姓名
    buyer_name = models.CharField(max_length=20, db_index=True)

    # 买方电话号码
    buyer_phone_number = models.CharField(max_length=20, db_index=True)

    # 买方身份证号码
    buyer_id = models.CharField(max_length=30, db_index=True)

    # 买方户口册图片，图片ID数组
    buyer_hukou_images = models.CharField(max_length=1000)

    # 买方身份证图片，图片ID数组
    buyer_id_images = models.CharField(max_length=1000)

    # 买方结婚证图片，图片ID数组
    buyer_marriage_images = models.CharField(max_length=1000)

    # 卖方姓名
    seller_name = models.CharField(max_length=20, db_index=True)

    # 卖方电话号码
    seller_phone_number = models.CharField(max_length=20, db_index=True)

    # 卖方身份证号码
    seller_id = models.CharField(max_length=30, db_index=True)

    # 卖方户口册图片，图片ID数组
    seller_hukou_images = models.CharField(max_length=1000)

    # 卖方身份证图片，图片ID数组
    seller_id_images = models.CharField(max_length=1000)

    # 卖方结婚证图片，图片ID数组
    seller_marriage_images = models.CharField(max_length=1000)

    # 承办人
    chengban_name = models.CharField(max_length=20, db_index=True, default='')

    # 网签合同
    wangqianhetong_images = models.CharField(max_length=1000, default='')

    # 备注
    remark = models.CharField(max_length=1000, default='')

    class Meta:
        db_table = "house"
