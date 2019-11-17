#include <chrono>
#include <iostream>
#include <receiver.hpp>
#include <sender.hpp>
#include <thread>

// Message definition
class Idle {};
class MoveSlow {};
class MoveFast {};
class StopMotion {};

using namespace std::chrono_literals;

void sender_work(messaging::Receiver& receiver) {
  // "Connect" sender and receiver by sharing the same queue
  messaging::Sender sender(receiver);

  // Change this value to see if the synchronization works.
  auto sleep_inverval = 10us;

  std::cout << "sending idle\n";
  sender.send(Idle());
  std::this_thread::sleep_for(sleep_inverval);
  std::cout << "sending moveslow\n";
  sender.send(MoveSlow());
  std::this_thread::sleep_for(sleep_inverval);
  std::cout << "sending movefast\n";
  sender.send(MoveFast());
  std::this_thread::sleep_for(sleep_inverval);
  std::cout << "sending stopmotion\n";
  sender.send(StopMotion());
  std::this_thread::sleep_for(sleep_inverval);
}

void receiver_work(messaging::Receiver& receiver) {
  // Receiver
  receiver.wait().handle<Idle>([](auto const& i) { std::cout << "Idle\n"; });
  receiver.wait().handle<MoveSlow>([](auto const& i) { std::cout << "MoveSlow\n"; });
  receiver.wait().handle<MoveFast>([](auto const& i) { std::cout << "MoveFast\n"; });
  receiver.wait().handle<StopMotion>([](auto const& i) { std::cout << "StopMotion\n"; });
}

int main(int argc, char* argv[]) {
  messaging::Receiver rec;

  std::thread sender_thread(&sender_work, std::ref(rec));
  std::thread receiver_thread(&receiver_work, std::ref(rec));

  sender_thread.join();
  receiver_thread.join();

  return 0;
}
