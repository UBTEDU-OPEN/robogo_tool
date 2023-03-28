#ifndef INFER_CONTACT_CLIENT_H
#define INFER_CONTACT_CLIENT_H

#include "infer_contact.grpc.pb.h"

#include <memory>
#include <string>

class InferContactClient
{
public:
    InferContactClient(std::shared_ptr<grpc::Channel> channel);
    ~InferContactClient();

    std::string execute(const std::string data);
    std::string init(const std::string data);
    std::string unload(const std::string data);

private:
    std::unique_ptr<InferContact::InferContactGrpc::Stub> m_stub;
};

#endif // INFER_CONTACT_CLIENT_H
