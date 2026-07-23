import os
from glob import glob
from setuptools import find_packages, setup

package_name = 'face_tracker'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        # Includes all launch files in the launch directory
        (os.path.join('share', package_name, 'launch'), glob(os.path.join('launch', '*launch.[pxy][yma]*'))),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='abinaabey',
    maintainer_email='abinaabey@todo.todo',
    description='TODO: Package description',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            # Maps the executable name to the Python node and main function
            'face_tracker = face_tracker.face_tracker_node:main'
        ],
    },
)
