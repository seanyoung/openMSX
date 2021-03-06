#ifndef CLICONNECTION_HH
#define CLICONNECTION_HH

#include "CliListener.hh"
#include "EventListener.hh"
#include "Socket.hh"
#include "CliComm.hh"
#include "AdhocCliCommParser.hh"
#include "Poller.hh"
#include <mutex>
#include <string>
#include <thread>

namespace openmsx {

class CommandController;
class EventDistributor;

class CliConnection : public CliListener, private EventListener
{
public:
	void setUpdateEnable(CliComm::UpdateType type, bool value) {
		updateEnabled[type] = value;
	}
	bool getUpdateEnable(CliComm::UpdateType type) const {
		return updateEnabled[type];
	}

	/** Starts the helper thread.
	  * Called when this CliConnection is added to GlobalCliComm (and
	  * after it's allowed to respond to external commands).
	  * Subclasses should themself send the opening tag (startOutput()).
	  */
	void start();

protected:
	CliConnection(CommandController& commandController,
	              EventDistributor& eventDistributor);
	~CliConnection();

	virtual void output(string_ref message) = 0;

	/** End this connection by sending the closing tag
	  * and then closing the stream.
	  * Subclasses should call this method at the start of their destructor.
	  */
	void end();

	/** Close the connection. After this method is called, calls to
	  * output() should be ignored.
	  */
	virtual void close() = 0;

	/** Send opening XML tag, should be called exactly once by a subclass
	  * shortly after opening a connection. Cannot be implemented in the
	  * base class because some subclasses (want to send data before this
	  * tag).
	  */
	void startOutput();

	AdhocCliCommParser parser;
	std::thread thread; // TODO: Possible to make this private?

private:
	virtual void run() = 0;

	void execute(const std::string& command);

	// CliListener
	void log(CliComm::LogLevel level, string_ref message) override;
	void update(CliComm::UpdateType type, string_ref machine,
	            string_ref name, string_ref value) override;

	// EventListener
	int signalEvent(const std::shared_ptr<const Event>& event) override;

	CommandController& commandController;
	EventDistributor& eventDistributor;

	bool updateEnabled[CliComm::NUM_UPDATES];
};

class StdioConnection final : public CliConnection
{
public:
	StdioConnection(CommandController& commandController,
	                EventDistributor& eventDistributor);
	~StdioConnection();

	void output(string_ref message) override;

private:
	void close() override;
	void run() override;

	Poller poller;
};

#ifdef _WIN32
class PipeConnection final : public CliConnection
{
public:
	PipeConnection(CommandController& commandController,
	               EventDistributor& eventDistributor,
	               string_ref name);
	~PipeConnection();

	void output(string_ref message) override;

private:
	void close() override;
	void run() override;

	HANDLE pipeHandle;
	HANDLE shutdownEvent;
};
#endif

class SocketConnection final : public CliConnection
{
public:
	SocketConnection(CommandController& commandController,
	                 EventDistributor& eventDistributor,
	                 SOCKET sd);
	~SocketConnection();

	void output(string_ref message) override;

private:
	void close() override;
	void run() override;
	void closeSocket();

	std::mutex sdMutex;
	SOCKET sd;
	Poller poller;
	bool established;
};

} // namespace openmsx

#endif
