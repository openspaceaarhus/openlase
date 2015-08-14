#ifndef MESSAGE_H_
#define MESSAGE_H_

#define HOSTADDR "tcp://localhost:5555"

struct message {
  int playerno;
  int action;

  message(int playerno, int action) 
  : playerno(playerno), action(action) {};

};

#endif /* !MESSAGE_H_ */
