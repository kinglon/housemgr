# 构造响应
def make_response_body(code=200, msg='', data=None):
    body = {
        'code': code,
        'message': msg
    }

    if data:
        body['data'] = data

    return body


# 构造响应体里面的数据列表
def make_response_data_list(total, data):
    data_list = {
        'total': total,
        'list': data
    }
    return data_list


# 检查是否为有效的整数数组，要求都是大于等于0的整数，逗号分隔
def check_int_array(int_array_string):
    try:
        nums = int_array_string.split(',')
        if len(nums) == 0:
            return False
        for num in nums:
            if int(num) < 0:
                return False
        return True
    except Exception:
        return False


# 将int型数组转成字符串
def int_array_to_string(int_array):
    int_array_string = '['
    for i in range(len(int_array)):
        if i > 0:
            int_array_string += ','
        int_array_string += str(int(int_array[i]))
    int_array_string += ']'
    return int_array_string


def test():
    print(int_array_to_string([]))
    print(int_array_to_string([1]))
    print(int_array_to_string([1, 2]))
    print(int_array_to_string(["1", "2"]))


if __name__ == '__main__':
    test()
