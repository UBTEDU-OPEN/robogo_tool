#include "infer_contact_client.h"
#include <grpc++/grpc++.h>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(infer_contact, "infer_contact");

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using InferContact::InferContactGrpc;


InferContactClient::InferContactClient(std::shared_ptr<grpc::Channel> channel)
    : m_stub(InferContactGrpc::NewStub(channel))
{
    qCDebug(infer_contact()) << __func__;
}

InferContactClient::~InferContactClient()
{
    qCDebug(infer_contact()) << __func__;
}

std::string InferContactClient::init(const std::string data)
{
    qCDebug(infer_contact()) << __func__ << " data: " <<  data.size();
    InferContact::data cmd_data, cmd_reply;
    cmd_data.set_buf(data);

    ClientContext context;

    Status status = m_stub->ai_init(&context, cmd_data, &cmd_reply);

    if (status.ok()) {
      return cmd_reply.buf();
    } else {
      qCDebug(infer_contact()) << static_cast<int32_t>(status.error_code()) << ": " << status.error_message().c_str();
      return "";
    }
}

std::string InferContactClient::execute(const std::string data)
{
    qCDebug(infer_contact()) << __func__ << " data: " <<  data.size();
    InferContact::data cmd_data, cmd_reply;
    cmd_data.set_buf(data);

    ClientContext context;

    Status status = m_stub->cmd_node(&context, cmd_data, &cmd_reply);

    if (status.ok()) {
      return cmd_reply.buf();
    } else {
      qCDebug(infer_contact()) << static_cast<int32_t>(status.error_code()) << ": " << status.error_message().c_str();
      return "";
    }
}

std::string InferContactClient::unload(const std::string data)
{
    qCDebug(infer_contact()) << __func__ << " data: " <<  data.size();
    InferContact::data cmd_data, cmd_reply;
    cmd_data.set_buf(data);

    ClientContext context;

    Status status = m_stub->unload(&context, cmd_data, &cmd_reply);

    if (status.ok()) {
      return cmd_reply.buf();
    } else {
      qCDebug(infer_contact()) << static_cast<int32_t>(status.error_code()) << ": " << status.error_message().c_str();
      return "";
    }
}
