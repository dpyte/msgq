#pragma once

#include <zmq.h>
#include <string>
#include <vector>

#include "msgq/ipc.h"

#define MAX_POLLERS 128

class ZMQContext : public Context {
private:
  void * context = nullptr;
public:
  ZMQContext();
  void * getRawContext() override {return context;}
  ~ZMQContext() override;
};

class ZMQMessage : public Message {
private:
  char * data;
  size_t size;
public:
  void init(size_t size) override;
  void init(char *data, size_t size) override;
  size_t getSize() override {return size;}
  char * getData() override {return data;}
  void close() override;
  ~ZMQMessage() override;
};

class ZMQSubSocket : public SubSocket {
private:
  void * sock;
  std::string full_endpoint;
public:
  int connect(Context *context, std::string endpoint, std::string address, bool conflate=false, bool check_endpoint=true);
  void setTimeout(int timeout) override;
  void * getRawSocket() override {return sock;}
  Message *receive(bool non_blocking=false) override;
  ~ZMQSubSocket() override;
};

class ZMQPubSocket : public PubSocket {
private:
  void * sock;
  std::string full_endpoint;
  int pid = -1;
public:
  int connect(Context *context, std::string endpoint, bool check_endpoint=true) override;
  int sendMessage(Message *message) override;
  int send(char *data, size_t size) override;
  bool all_readers_updated() override;
  ~ZMQPubSocket() override;
};

class ZMQPoller : public Poller {
private:
  std::vector<SubSocket*> sockets;
  zmq_pollitem_t polls[MAX_POLLERS];
  size_t num_polls = 0;

public:
  void registerSocket(SubSocket *socket) override;
  std::vector<SubSocket*> poll(int timeout) override;
  ~ZMQPoller() override = default;
};
