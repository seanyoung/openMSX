// $Id$

#ifndef INTERPRETER_HH
#define INTERPRETER_HH

#include "EventListener.hh"
#include "noncopyable.hh"
#include <map>
#include <set>
#include <string>
#include <vector>
#include <tcl.h>

namespace openmsx {

class EventDistributor;
class Reactor;
class Command;
class Setting;
class InterpreterOutput;
class TclObject;

class Interpreter : private EventListener, private noncopyable
{
public:
	Interpreter(EventDistributor& eventDistributor, Reactor& reactor);
	~Interpreter();

	void setOutput(InterpreterOutput* output);

	void init(const char* programName);
	void registerCommand(const std::string& name, Command& command);
	void unregisterCommand(const std::string& name, Command& command);
	void getCommandNames(std::set<std::string>& result);
	bool isComplete(const std::string& command) const;
	std::string execute(const std::string& command);
	std::string executeFile(const std::string& filename);

	void setVariable(const std::string& name, const std::string& value);
	void unsetVariable(const std::string& name);
	const char* getVariable(const std::string& name) const;
	void registerSetting(Setting& variable, const std::string& name);
	void unregisterSetting(Setting& variable, const std::string& name);

	void createNamespace(const std::string& name);
	void deleteNamespace(const std::string& name);

	void splitList(const std::string& list,
	               std::vector<std::string>& result);
	static void splitList(const std::string& list,
	                      std::vector<std::string>& result,
	                      Tcl_Interp* interp);

	void registerProxySetting(const std::string& name);
	void unregisterProxySetting(const std::string& name);

private:
	// EventListener
	virtual bool signalEvent(shared_ptr<const Event> event);

	void poll();

	static int outputProc(ClientData clientData, const char* buf,
	        int toWrite, int* errorCodePtr);
	static int commandProc(ClientData clientData, Tcl_Interp* interp,
                               int objc, Tcl_Obj* const objv[]);
	static char* traceProc(ClientData clientData, Tcl_Interp* interp,
                const char* part1, const char* part2, int flags);

	Setting* getMachineSetting(const std::string& name);
	static char* proxyTraceProc(ClientData clientData, Tcl_Interp* interp,
                const char* part1, const char* part2, int flags);

	EventDistributor& eventDistributor;
	Reactor& reactor;

	static Tcl_ChannelType channelType;
	Tcl_Interp* interp;
	typedef std::map<std::string, Tcl_Command> CommandTokenMap;
	CommandTokenMap commandTokenMap;
	InterpreterOutput* output;

	friend class TclObject;
};

} // namespace openmsx

#endif
