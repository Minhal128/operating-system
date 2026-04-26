#ifndef ATTACK_SIMULATOR_H
#define ATTACK_SIMULATOR_H

#include "../monitoring/monitor.h"
#include <string>

namespace edr {

class AttackSimulator {
public:
    AttackSimulator(Monitor& monitor);
    ~AttackSimulator();

    // Attack simulations
    void simulateForkBomb(size_t process_count);
    void simulateFileSpam(const std::string& directory, size_t file_count);
    void simulateMemoryExhaustion(size_t mb_to_allocate);
    void simulateIPCFlood(size_t message_count);
    
    // Normal operations
    void simulateNormalActivity();
    void simulateAllAttacks();

private:
    Monitor& monitor_;
};

} // namespace edr

#endif // ATTACK_SIMULATOR_H
