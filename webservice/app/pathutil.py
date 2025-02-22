import os

from .setting import Setting

g_data_path = Setting().data_path


class PathUtil:
    @staticmethod
    def get_image_path():
        image_path = os.path.join(g_data_path, 'image')
        os.makedirs(image_path, exist_ok=True)
        return image_path

    @staticmethod
    def get_cache_path():
        cache_path = os.path.join(g_data_path, 'cache')
        os.makedirs(cache_path, exist_ok=True)
        return cache_path
