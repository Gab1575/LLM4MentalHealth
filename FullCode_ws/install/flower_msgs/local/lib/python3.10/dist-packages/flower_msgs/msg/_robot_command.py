# generated from rosidl_generator_py/resource/_idl.py.em
# with input from flower_msgs:msg/RobotCommand.idl
# generated code does not contain a copyright notice


# Import statements for member types

import builtins  # noqa: E402, I100

import math  # noqa: E402, I100

# Member 'servo_angles'
# Member 'led_colours_hex'
# Member 'led_colours_brightness'
import numpy  # noqa: E402, I100

import rosidl_parser.definition  # noqa: E402, I100


class Metaclass_RobotCommand(type):
    """Metaclass of message 'RobotCommand'."""

    _CREATE_ROS_MESSAGE = None
    _CONVERT_FROM_PY = None
    _CONVERT_TO_PY = None
    _DESTROY_ROS_MESSAGE = None
    _TYPE_SUPPORT = None

    __constants = {
    }

    @classmethod
    def __import_type_support__(cls):
        try:
            from rosidl_generator_py import import_type_support
            module = import_type_support('flower_msgs')
        except ImportError:
            import logging
            import traceback
            logger = logging.getLogger(
                'flower_msgs.msg.RobotCommand')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__robot_command
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__robot_command
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__robot_command
            cls._TYPE_SUPPORT = module.type_support_msg__msg__robot_command
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__robot_command

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
        }


class RobotCommand(metaclass=Metaclass_RobotCommand):
    """Message class 'RobotCommand'."""

    __slots__ = [
        '_servo_angles',
        '_n20_pwm',
        '_n20_target_rotations',
        '_led_colours_hex',
        '_led_colours_brightness',
    ]

    _fields_and_field_types = {
        'servo_angles': 'uint8[5]',
        'n20_pwm': 'int16',
        'n20_target_rotations': 'float',
        'led_colours_hex': 'uint32[5]',
        'led_colours_brightness': 'int16[5]',
    }

    SLOT_TYPES = (
        rosidl_parser.definition.Array(rosidl_parser.definition.BasicType('uint8'), 5),  # noqa: E501
        rosidl_parser.definition.BasicType('int16'),  # noqa: E501
        rosidl_parser.definition.BasicType('float'),  # noqa: E501
        rosidl_parser.definition.Array(rosidl_parser.definition.BasicType('uint32'), 5),  # noqa: E501
        rosidl_parser.definition.Array(rosidl_parser.definition.BasicType('int16'), 5),  # noqa: E501
    )

    def __init__(self, **kwargs):
        assert all('_' + key in self.__slots__ for key in kwargs.keys()), \
            'Invalid arguments passed to constructor: %s' % \
            ', '.join(sorted(k for k in kwargs.keys() if '_' + k not in self.__slots__))
        if 'servo_angles' not in kwargs:
            self.servo_angles = numpy.zeros(5, dtype=numpy.uint8)
        else:
            self.servo_angles = numpy.array(kwargs.get('servo_angles'), dtype=numpy.uint8)
            assert self.servo_angles.shape == (5, )
        self.n20_pwm = kwargs.get('n20_pwm', int())
        self.n20_target_rotations = kwargs.get('n20_target_rotations', float())
        if 'led_colours_hex' not in kwargs:
            self.led_colours_hex = numpy.zeros(5, dtype=numpy.uint32)
        else:
            self.led_colours_hex = numpy.array(kwargs.get('led_colours_hex'), dtype=numpy.uint32)
            assert self.led_colours_hex.shape == (5, )
        if 'led_colours_brightness' not in kwargs:
            self.led_colours_brightness = numpy.zeros(5, dtype=numpy.int16)
        else:
            self.led_colours_brightness = numpy.array(kwargs.get('led_colours_brightness'), dtype=numpy.int16)
            assert self.led_colours_brightness.shape == (5, )

    def __repr__(self):
        typename = self.__class__.__module__.split('.')
        typename.pop()
        typename.append(self.__class__.__name__)
        args = []
        for s, t in zip(self.__slots__, self.SLOT_TYPES):
            field = getattr(self, s)
            fieldstr = repr(field)
            # We use Python array type for fields that can be directly stored
            # in them, and "normal" sequences for everything else.  If it is
            # a type that we store in an array, strip off the 'array' portion.
            if (
                isinstance(t, rosidl_parser.definition.AbstractSequence) and
                isinstance(t.value_type, rosidl_parser.definition.BasicType) and
                t.value_type.typename in ['float', 'double', 'int8', 'uint8', 'int16', 'uint16', 'int32', 'uint32', 'int64', 'uint64']
            ):
                if len(field) == 0:
                    fieldstr = '[]'
                else:
                    assert fieldstr.startswith('array(')
                    prefix = "array('X', "
                    suffix = ')'
                    fieldstr = fieldstr[len(prefix):-len(suffix)]
            args.append(s[1:] + '=' + fieldstr)
        return '%s(%s)' % ('.'.join(typename), ', '.join(args))

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return False
        if all(self.servo_angles != other.servo_angles):
            return False
        if self.n20_pwm != other.n20_pwm:
            return False
        if self.n20_target_rotations != other.n20_target_rotations:
            return False
        if all(self.led_colours_hex != other.led_colours_hex):
            return False
        if all(self.led_colours_brightness != other.led_colours_brightness):
            return False
        return True

    @classmethod
    def get_fields_and_field_types(cls):
        from copy import copy
        return copy(cls._fields_and_field_types)

    @builtins.property
    def servo_angles(self):
        """Message field 'servo_angles'."""
        return self._servo_angles

    @servo_angles.setter
    def servo_angles(self, value):
        if isinstance(value, numpy.ndarray):
            assert value.dtype == numpy.uint8, \
                "The 'servo_angles' numpy.ndarray() must have the dtype of 'numpy.uint8'"
            assert value.size == 5, \
                "The 'servo_angles' numpy.ndarray() must have a size of 5"
            self._servo_angles = value
            return
        if __debug__:
            from collections.abc import Sequence
            from collections.abc import Set
            from collections import UserList
            from collections import UserString
            assert \
                ((isinstance(value, Sequence) or
                  isinstance(value, Set) or
                  isinstance(value, UserList)) and
                 not isinstance(value, str) and
                 not isinstance(value, UserString) and
                 len(value) == 5 and
                 all(isinstance(v, int) for v in value) and
                 all(val >= 0 and val < 256 for val in value)), \
                "The 'servo_angles' field must be a set or sequence with length 5 and each value of type 'int' and each unsigned integer in [0, 255]"
        self._servo_angles = numpy.array(value, dtype=numpy.uint8)

    @builtins.property
    def n20_pwm(self):
        """Message field 'n20_pwm'."""
        return self._n20_pwm

    @n20_pwm.setter
    def n20_pwm(self, value):
        if __debug__:
            assert \
                isinstance(value, int), \
                "The 'n20_pwm' field must be of type 'int'"
            assert value >= -32768 and value < 32768, \
                "The 'n20_pwm' field must be an integer in [-32768, 32767]"
        self._n20_pwm = value

    @builtins.property
    def n20_target_rotations(self):
        """Message field 'n20_target_rotations'."""
        return self._n20_target_rotations

    @n20_target_rotations.setter
    def n20_target_rotations(self, value):
        if __debug__:
            assert \
                isinstance(value, float), \
                "The 'n20_target_rotations' field must be of type 'float'"
            assert not (value < -3.402823466e+38 or value > 3.402823466e+38) or math.isinf(value), \
                "The 'n20_target_rotations' field must be a float in [-3.402823466e+38, 3.402823466e+38]"
        self._n20_target_rotations = value

    @builtins.property
    def led_colours_hex(self):
        """Message field 'led_colours_hex'."""
        return self._led_colours_hex

    @led_colours_hex.setter
    def led_colours_hex(self, value):
        if isinstance(value, numpy.ndarray):
            assert value.dtype == numpy.uint32, \
                "The 'led_colours_hex' numpy.ndarray() must have the dtype of 'numpy.uint32'"
            assert value.size == 5, \
                "The 'led_colours_hex' numpy.ndarray() must have a size of 5"
            self._led_colours_hex = value
            return
        if __debug__:
            from collections.abc import Sequence
            from collections.abc import Set
            from collections import UserList
            from collections import UserString
            assert \
                ((isinstance(value, Sequence) or
                  isinstance(value, Set) or
                  isinstance(value, UserList)) and
                 not isinstance(value, str) and
                 not isinstance(value, UserString) and
                 len(value) == 5 and
                 all(isinstance(v, int) for v in value) and
                 all(val >= 0 and val < 4294967296 for val in value)), \
                "The 'led_colours_hex' field must be a set or sequence with length 5 and each value of type 'int' and each unsigned integer in [0, 4294967295]"
        self._led_colours_hex = numpy.array(value, dtype=numpy.uint32)

    @builtins.property
    def led_colours_brightness(self):
        """Message field 'led_colours_brightness'."""
        return self._led_colours_brightness

    @led_colours_brightness.setter
    def led_colours_brightness(self, value):
        if isinstance(value, numpy.ndarray):
            assert value.dtype == numpy.int16, \
                "The 'led_colours_brightness' numpy.ndarray() must have the dtype of 'numpy.int16'"
            assert value.size == 5, \
                "The 'led_colours_brightness' numpy.ndarray() must have a size of 5"
            self._led_colours_brightness = value
            return
        if __debug__:
            from collections.abc import Sequence
            from collections.abc import Set
            from collections import UserList
            from collections import UserString
            assert \
                ((isinstance(value, Sequence) or
                  isinstance(value, Set) or
                  isinstance(value, UserList)) and
                 not isinstance(value, str) and
                 not isinstance(value, UserString) and
                 len(value) == 5 and
                 all(isinstance(v, int) for v in value) and
                 all(val >= -32768 and val < 32768 for val in value)), \
                "The 'led_colours_brightness' field must be a set or sequence with length 5 and each value of type 'int' and each integer in [-32768, 32767]"
        self._led_colours_brightness = numpy.array(value, dtype=numpy.int16)
