import os.path
import shutil
import subprocess
import sys
import time
from datetime import datetime
from logutil import LogUtil

# 把目录加入到搜索路径
root_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
sys.path.append(root_dir)
from app.setting import Setting


g_setting = Setting()
g_image_path = os.path.join(g_setting.data_path, 'image')
g_image_backup_path = os.path.join(g_setting.backup_path, 'image')
os.makedirs(g_image_backup_path, exist_ok=True)
g_database_backup_path = os.path.join(g_setting.backup_path, 'database')
os.makedirs(g_database_backup_path, exist_ok=True)


def backup_images():
    # 读取上一次备份的时间
    last_backup_time_file_path = os.path.join(g_image_backup_path, 'last_backup_time.txt')
    last_backup_time = 0.0
    if os.path.exists(last_backup_time_file_path):
        with open(last_backup_time_file_path, 'r') as f:
            last_backup_time = float(f.read().strip())

    # 扫描原始图片目录
    image_file_paths = []
    max_create_time = last_backup_time
    for root, _, files in os.walk(g_image_path):
        for file in files:
            file_path = os.path.join(root, file)
            creation_time = os.path.getctime(file_path)
            if creation_time > last_backup_time:
                image_file_paths.append(file_path)
                if creation_time > max_create_time:
                    max_create_time = creation_time
    print('需要备份文件数：{}'.format(len(image_file_paths)))

    # 复制新文件到备份目录
    for file_path in image_file_paths:
        relative_path = os.path.relpath(file_path, g_image_path)
        backup_path = os.path.join(g_image_backup_path, relative_path)
        os.makedirs(os.path.dirname(backup_path), exist_ok=True)
        shutil.copy2(file_path, backup_path)
        print(f'复制文件: {file_path} -> {backup_path}')

    # 保存备份时间
    with open(last_backup_time_file_path, 'w') as f:
        f.write(str(max_create_time))


def backup_database():
    """备份数据库"""
    # 生成备份文件名（按时间戳命名）
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
    backup_file = os.path.join(g_database_backup_path, f'house_backup_{timestamp}.sql')

    # 使用 mysqldump 导出数据库
    try:
        command = [
            'mysqldump',
            f'--host=127.0.0.1',
            f'--user={g_setting.database_user}',
            f'--password={g_setting.database_password}',
            'house',
            '--result-file=' + backup_file
        ]
        subprocess.run(command, check=True)
        print(f"数据库备份成功，保存到: {backup_file}")
    except subprocess.CalledProcessError as e:
        print(f"数据库备份失败，错误: {e}")


def main():
    next_backup_time = 0
    while True:
        now = datetime.now().timestamp()
        if now < next_backup_time:
            print('等待下一次备份时间')
            time.sleep(60)
            continue

        try:
            print('开始备份图片')
            backup_images()
            print('备份图片完成')
        except Exception as e:
            print('备份图片遇到问题：{}'.format(e))

        try:
            print('开始备份数据库')
            backup_database()
            print('备份数据库完成')
        except Exception as e:
            print('备份数据库遇到问题：{}'.format(e))

        now = datetime.now()
        today = datetime.strptime('{}-{}-{}'.format(now.year, now.month, now.day), '%Y-%m-%d')
        next_backup_time = int(today.timestamp()) + 24*3600


if __name__ == '__main__':
    LogUtil.file_name_prefix = 'backuptool'
    LogUtil.enable()
    print('运行备份工具')
    main()
