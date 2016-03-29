#ifndef ETRADECLIENT_HARDWARE_HARDWARE_CMD_H_INCLUDED
#define ETRADECLIENT_HARDWARE_HARDWARE_CMD_H_INCLUDED

#include <string>

#include <boost/property_tree/ptree.hpp>

class HardwareCmd
{
	// Return a boost ptree node as a reply data, we don't return reply as "std::string" type because
	// it will cause some trouble when construct a result JSON string if the data is alreay a JSON string.
	// It will add redundant back slash character ("\") in the JSON string.
	typedef boost::property_tree::ptree ReplyData;
public:
	struct Reply
	{
		ReplyData data;
		std::string error_code;
	};

	virtual ~HardwareCmd() {}

	/*Exception will be thrown if any error happens.*/
	virtual Reply Execute(const std::string& input) = 0;
};

class ReadPINPadCmd : public HardwareCmd
{
public:
	virtual Reply Execute(const std::string& input) override;
};

class ReadIDCardCmd : public HardwareCmd
{
public:
	virtual Reply Execute(const std::string& input) override;
};

class ActivateDILICardCmd : public HardwareCmd
{
public:
	virtual Reply Execute(const std::string& input) override;
};

class ResetDILICardCmd : public HardwareCmd
{
public:
	virtual Reply Execute(const std::string& input) override;
};

class ReadDILICardBasicInfoCmd : public HardwareCmd
{
public:
	virtual Reply Execute(const std::string& input) override;
};

class ReadDILICardServiceInfoCmd : public HardwareCmd
{
public:
	virtual Reply Execute(const std::string& input) override;
};

class WriteDILICardServiceInfoCmd : public HardwareCmd
{
public:
	virtual Reply Execute(const std::string& input) override;
};

#endif // ETRADECLIENT_HARDWARE_HARDWARE_CMD_H_INCLUDED
