#ifndef JSONRPC_LEAN_THREAD_SAFE_RPC_QUEUE_H
#define JSONRPC_LEAN_THREAD_SAFE_RPC_QUEUE_H

#include <condition_variable> 
#include <iostream> 
#include <mutex> 
#include <queue>

#include "formatteddata.h"
#include "jsonformatteddata.h"

  
// Thread-safe queue 
class TsRpcQueue { 
private: 
    // Underlying queue 
    std::queue<std::shared_ptr<jsonrpc::FormattedData>> m_queue; 
  
    // mutex for thread synchronization 
    std::mutex m_mutex; 
  
    // Condition variable for signaling 
    std::condition_variable m_cond; 
  
public: 
    // Pushes an element to the queue 
    void push(std::shared_ptr<jsonrpc::FormattedData> item) 
    { 
        // Acquire lock 
        std::unique_lock<std::mutex> lock(m_mutex); 
  
        // Add item 
        m_queue.push(item); 
  
        // Notify one thread that 
        // is waiting 
        m_cond.notify_one(); 
    } 
  
    // Pops an element off the queue 
    std::shared_ptr<jsonrpc::FormattedData> pop() 
    { 
  
        // acquire lock 
        std::unique_lock<std::mutex> lock(m_mutex); 
  
        // wait until queue is not empty 
        m_cond.wait(lock, 
                    [this]() { return !m_queue.empty(); }); 
  
        // retrieve item 
        std::shared_ptr<jsonrpc::FormattedData> item = m_queue.front(); 
        m_queue.pop(); 
  
        // return item 
        return item; 
    } 
};

#endif // JSONRPC_LEAN_THREAD_SAFE_RPC_QUEUE_H