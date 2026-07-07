from setuptools import find_packages
from setuptools import setup

setup(
    name='flower_msgs',
    version='0.0.0',
    packages=find_packages(
        include=('flower_msgs', 'flower_msgs.*')),
)
