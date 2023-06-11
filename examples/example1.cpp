#include <iostream>
#include <ringen/ring_buffer.hpp>
#include <ringen/sequence_barrier.hpp>
#include <ringen/single_threaded_claim_strategy.hpp>
#include <ringen/spin_wait_strategy.hpp>
#include <thread>

using namespace ringen;

struct event {
  uint32_t data;
};

int main() {
  constexpr size_t buffer_size = 1024;  // Must be power-of-two!

  ring_buffer<event> buffer(buffer_size);

  spin_wait_strategy wait_strategy;
  sequence_barrier<spin_wait_strategy> consumed(wait_strategy);

  single_threaded_claim_strategy<spin_wait_strategy> claim_strategy(
      buffer_size, wait_strategy);

  claim_strategy.add_claim_barrier(consumed);

  // The consumer thread lambda function.
  std::thread consumer([&]() {
    uint64_t sum = 0;
    sequence_t nextToRead = 0;
    bool done = false;

    while(!done) {
      // Wait until more items are available.
      sequence_t available = claim_strategy.wait_until_published(nextToRead);

      // Process all available items in a batch
      do {
        auto& event = buffer[nextToRead];
        sum += event.data;
        if(event.data == 0) {
          done = true;
        }
      } while(nextToRead++ != available);

      // Notify producer we've finished consuming items.
      consumed.publish(available);

      std::cout << "sum is " << sum << std::endl; // Where to move print?   
    }
  });

  // The producer thread lambda function.
  std::thread producer([&]() {
    for(uint32_t i = 1; i <= 1'000'000 ; ++i) {
      // Claim a slot in the ring buffer, waits if buffer is full.
      sequence_t seq = claim_strategy.claim_one();

      // Write to the slot in the ring buffer.
      buffer[seq].data = i;

      // Publish the event to the consumer.
      claim_strategy.publish(seq);
    }

    // Publish the terminating event.
    sequence_t seq = claim_strategy.claim_one();
    buffer[seq].data = 0;
    claim_strategy.publish(seq);
  });

  consumer.join();
  producer.join();

  return 0;
}