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
  constexpr size_t buffer_size = 1024 * 2; // Must be power-of-two!

  ring_buffer<event> buffer(buffer_size);

  spin_wait_strategy wait_strategy;
  sequence_barrier<spin_wait_strategy> barrier(wait_strategy);

  single_threaded_claim_strategy<spin_wait_strategy> claim_strategy(
      buffer_size, wait_strategy);

  claim_strategy.add_claim_barrier(barrier);

  // The consumer thread lambda function.
  std::thread consumer([&]() {
    uint64_t sum = 0;
    sequence_t next_to_read = 0;
    bool done = false;

    while (!done) {
      // Wait until more items are available.
      sequence_t available = claim_strategy.wait_until_published(next_to_read);

      // Process all available items in a batch.
      do {
        auto &event = buffer[next_to_read];
        sum += event.data;
        if (event.data == 0) {
          done = true;
        }
      } while (next_to_read++ != available);

      // Notify producer we've finished consuming items.
      barrier.publish(available);
      std::cerr << "consumer: sum = " << sum
                << "\r"; //  TODO Move print outside?
    }
  });

  // The producer thread lambda function.
  std::thread producer([&]() {
    for (uint32_t i = 1; i <= 1'000'000; ++i) {
      // Claim a slot in the ring buffer, waits if buffer is full.
      sequence_t sequence = claim_strategy.claim_one();
      // Write to the slot in the ring buffer.
      buffer[sequence].data = i;
      // Publish the event to the consumer.
      claim_strategy.publish(sequence);
      // std::cout << "produced: data = " <<  i << std::endl;
    }
    // Publish the terminating event.
    sequence_t sequence = claim_strategy.claim_one();
    buffer[sequence].data = 0;
    claim_strategy.publish(sequence);
  });

  consumer.join();
  producer.join();

  return 0;
}