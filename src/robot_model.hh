#ifndef DYNAMIC_GRAPH_BRIDGE_ROBOT_MODEL_HH
# define DYNAMIC_GRAPH_BRIDGE_ROBOT_MODEL_HH
# include <list>
# include <string>

# include "jrl/mal/matrixabstractlayer.hh"

# include <jrl/dynamics/dynamicsfactory.hh>

//# include <sot/core/flags.hh>
# include <dynamic-graph/command.h>
# include <dynamic-graph/entity.h>
//# include <dynamic-graph/pool.h>
# include <dynamic-graph/signal-ptr.h>
# include <dynamic-graph/signal-time-dependent.h>
//# include <sot/core/exception-dynamic.hh>
# include <sot/core/matrix-homogeneous.hh>

namespace dynamicgraph
{
  class RosRobotModel;

  namespace command
  {
    using ::dynamicgraph::command::Command;
    using ::dynamicgraph::command::Value;

    /// \brief Load from the robot_description parameter of the
    /// parameter server.
    ///
    /// This is the recommended method as it ensures model consistency
    /// between the control and the other nodes.
    class LoadFromParameterServer : public Command
    {
    public:
      explicit LoadFromParameterServer(RosRobotModel& entity,
				       const std::string& docstring);
      Value doExecute();
    };

    /// \brief Load model from an URDF file.
    class LoadUrdf : public Command
    {
    public:
      explicit LoadUrdf(RosRobotModel& entity, const std::string& docstring);
      Value doExecute();
    };
  } // end of namespace command.

  /// \brief This entity load either the current model available in
  /// the robot_description parameter or a specified file and provides
  /// various data such as body positions, jacobians, etc.
  ///
  /// This relies on jrl_dynamics_urdf to load the model and jrl-dynamics
  /// to realize the computation.
  class RosRobotModel : public dynamicgraph::Entity
  {
    DYNAMIC_GRAPH_ENTITY_DECL();
  public:
    typedef ::dynamicgraph::sot::MatrixHomogeneous MatrixHomogeneous;
    RosRobotModel(const std::string& n);
    virtual ~RosRobotModel();

    void loadUrdf(const std::string& filename);
    void loadFromParameterServer();

  protected:
    void buildSignals();

    /// \brief Callback Computing the position of the body attached
    /// to the provided joint.
    ///
    MatrixHomogeneous&
    computeBodyPosition (CjrlJoint* joint,
			 MatrixHomogeneous& position,
			 int t);

    /// \brief Update data if necessary by updating the robot
    /// configuration/velocity/acceleration.
    ///
    /// \param t current time
    void update (int t);

    unsigned getDimension () const
    {
      if (!robot_)
	throw std::runtime_error ("no robot loaded");
      return robot_->numberDof();
    }

    dynamicgraph::Vector& computeZmp (dynamicgraph::Vector& zmp, int time);
    dynamicgraph::Vector& computeCom (dynamicgraph::Vector& com, int time);
    dynamicgraph::Matrix& computeJCom (dynamicgraph::Matrix& jcom, int time);

    dynamicgraph::Vector& computeLowerJointLimits (dynamicgraph::Vector&, int time);
    dynamicgraph::Vector& computeUpperJointLimits (dynamicgraph::Vector&, int time);

  private:
    CjrlHumanoidDynamicRobot* robot_;
    std::list< ::dynamicgraph::SignalBase<int>* > genericSignalRefs_;

    /// \brief When did the last computation occur?
    int lastComputation_;

    /// \brief Robot current configuration.
    dynamicgraph::SignalPtr<dynamicgraph::Vector,int> q_;
    /// \brief Robot current velocity.
    dynamicgraph::SignalPtr<dynamicgraph::Vector,int> dq_;
    /// \brief Robot current acceleration.
    dynamicgraph::SignalPtr<dynamicgraph::Vector,int> ddq_;

    /// \brief Zero Momentum Point
    dynamicgraph::SignalTimeDependent<dynamicgraph::Vector,int> zmp_;
    /// \brief Center of mass
    dynamicgraph::SignalTimeDependent<dynamicgraph::Vector,int> com_;
    /// \brief Center of mass jacobian
    dynamicgraph::SignalTimeDependent<dynamicgraph::Matrix,int> jcom_;

    /// \brief Lower joints limits
    dynamicgraph::SignalTimeDependent<dynamicgraph::Vector,int> lowerJointLimits_;
    /// \brief Upper joints limits
    dynamicgraph::SignalTimeDependent<dynamicgraph::Vector,int> upperJointLimits_;
  };
} // end of namespace dynamicgraph.

#endif //! DYNAMIC_GRAPH_BRIDGE_ROBOT_MODEL_HH
