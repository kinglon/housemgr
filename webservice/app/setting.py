import os


class Setting:
    def __init__(self):
        # data根目录
        self.data_path = ''

        self.load()

    def load(self):
        current_file_path = os.path.dirname(os.path.abspath(__file__))

        # 加载设置
        config_file_path = os.path.join(current_file_path, r'..\configs\configs.txt')
        with open(config_file_path, 'r', encoding='utf-8') as file:
            for line in file:
                # Remove newline characters and process each line
                processed_line = line.strip()
                if processed_line.find('#') == 0:
                    # 注释行忽略
                    continue
                parts = processed_line.split('=')
                if len(parts) != 2:
                    continue
                key = parts[0].strip()
                value = parts[1].strip()
                if key == 'data_path':
                    self.data_path = value
