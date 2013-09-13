from ros_import import RosImport
from ros_export import RosExport
from ros_joint_state import RosJointState
from ros_time import RosTime

from dynamic_graph import plug

class Ros(object):
    device = None
    rosImport = None
    rosExport = None
    rosJointState = None

    def __init__(self, robot, suffix = ''):
        self.robot = robot
        self.rosImport = RosImport('rosImport{0}'.format(suffix))
        self.rosExport = RosExport('rosExport{0}'.format(suffix))
        self.rosJointState = RosJointState('rosJointState{0}'.format(suffix))
        self.rosJointState.retrieveJointNames(self.robot.dynamic.name)
        self.rosTime = RosTime ('rosTime{0}'.format(suffix))

        plug(self.robot.device.state, self.rosJointState.state)
        self.robot.device.after.addSignal(
            '{0}.trigger'.format(self.rosImport.name))
        self.robot.device.after.addSignal(
            '{0}.trigger'.format(self.rosJointState.name))
