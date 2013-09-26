#ifndef DYNAMIC_GRAPH_ROS_PUBLISH_HH
# define DYNAMIC_GRAPH_ROS_PUBLISH_HH
# include <iostream>
# include <map>

# include <boost/shared_ptr.hpp>
# include <boost/tuple/tuple.hpp>

# include <dynamic-graph/entity.h>
# include <dynamic-graph/signal-time-dependent.h>
# include <dynamic-graph/command.h>

# include <ros/ros.h>

# include <realtime_tools/realtime_publisher.h>

# include "converter.hh"
# include "sot_to_ros.hh"

namespace dynamicgraph
{
  class RosPublish;

  namespace command
  {
    namespace rosPublish
    {
      using ::dynamicgraph::command::Command;
      using ::dynamicgraph::command::Value;

# define ROS_PUBLISH_MAKE_COMMAND(CMD)			\
      class CMD : public Command			\
      {							\
      public:						\
	CMD (RosPublish& entity,				\
	     const std::string& docstring);		\
	virtual Value doExecute ();			\
      }

      ROS_PUBLISH_MAKE_COMMAND(Add);
      ROS_PUBLISH_MAKE_COMMAND(Clear);
      ROS_PUBLISH_MAKE_COMMAND(List);
      ROS_PUBLISH_MAKE_COMMAND(Rm);

#undef ROS_PUBLISH_MAKE_COMMAND

    } // end of namespace errorEstimator.
  } // end of namespace command.


  /// \brief Publish dynamic-graph information into ROS.
  class RosPublish : public dynamicgraph::Entity
  {
    DYNAMIC_GRAPH_ENTITY_DECL();
  public:
    typedef boost::function<void (int)> callback_t;

    typedef boost::tuple<
      boost::shared_ptr<dynamicgraph::SignalBase<int> >,
      callback_t>
    bindedSignal_t;

    static const double ROS_JOINT_STATE_PUBLISHER_RATE = 1. / 100.;

    RosPublish (const std::string& n);
    virtual ~RosPublish ();

    virtual std::string getDocString () const;
    void display (std::ostream& os) const;

    void add (const std::string& signal, const std::string& topic);
    void rm (const std::string& signal);
    std::string list () const;
    void clear ();

    int& trigger (int&, int);

    template <typename T>
    void
    sendData
    (boost::shared_ptr
     <realtime_tools::RealtimePublisher
     <typename SotToRos<T>::ros_t> > publisher,
     boost::shared_ptr<typename SotToRos<T>::signalIn_t> signal,
     int time);

    template <typename T>
    void add (const std::string& signal, const std::string& topic);

  private:
    static const std::string docstring_;
    ros::NodeHandle& nh_;
    std::map<std::string, bindedSignal_t> bindedSignal_;
    dynamicgraph::SignalTimeDependent<int,int> trigger_;
    ros::Duration rate_;
    ros::Time lastPublicated_;
  };
} // end of namespace dynamicgraph.

# include "ros_publish.hxx"
#endif //! DYNAMIC_GRAPH_ROS_PUBLISH_HH
