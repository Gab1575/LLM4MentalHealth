from setuptools import find_packages, setup

package_name = 'flower_mux'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='gavin',
    maintainer_email='Gavin.Bayley@queensu.ca',
    description='Mode-dependent command multiplexer for the flower robot.',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'flower_mux = flower_mux.flower_mux_node:main'
        ],
    },
)
