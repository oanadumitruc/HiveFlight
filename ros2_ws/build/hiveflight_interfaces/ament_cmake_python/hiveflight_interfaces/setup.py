from setuptools import find_packages
from setuptools import setup

setup(
    name='hiveflight_interfaces',
    version='0.0.1',
    packages=find_packages(
        include=('hiveflight_interfaces', 'hiveflight_interfaces.*')),
)
