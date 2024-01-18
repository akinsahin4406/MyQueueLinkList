#include <string>
#include <iostream>
#include <bits/stdc++.h>
#include <memory>
#include <condition_variable>

template<typename T>
struct Node {
    T data;
    Node* next;
    Node(T new_val):
    data{new_val},
    next{nullptr}
    {
    }
};

template<typename T>
class MyQueueLinkList
{
    private:
    Node<T>* front;
    Node<T>* rear;
    std::atomic<int> count;
    std::mutex m_mutex;
    std::condition_variable cv;
    void enQueue(T &&new_value);
    void deQueue();
    T& getFront() const;
    public:
    MyQueueLinkList():
    front{nullptr},
    rear{nullptr},
    count{0}
    {}
    void push(T&& new_value);
    bool empty()
    {
        return count <= 0;
    }
    bool pop_and_get(T& get_value);
    void pop_and_get_consumer(T& get_value);
};

template<typename T>
void MyQueueLinkList<T>::enQueue(T &&new_value)
{
    Node<T>* new_node = new Node<T>(new_value);
    ++count;
    if (rear == nullptr)
    {
        front = new_node;
        rear = new_node;
        return;
    }

    rear->next = new_node;
    rear = new_node;
}

template<typename T>
void MyQueueLinkList<T>::deQueue()
{
    if (front == nullptr)
    {
        return;
    }
    --count;
    if (rear == front)
    {
        delete rear;
        rear = nullptr;
        front = nullptr;
        return;
    }

    Node<T>* delete_this = front;
    front = front->next;
    delete delete_this;
}

template<typename T>
T& MyQueueLinkList<T>::getFront() const
{
    return front->data;
}

template<typename T>
void  MyQueueLinkList<T>::push(T&& new_value)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    this->enQueue(std::move(new_value));
    lock.unlock();
    cv.notify_one();
}

template<typename T>
bool MyQueueLinkList<T>::pop_and_get(T& get_value)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (empty())
    {
        return false;
    }
    get_value = getFront();
    deQueue();
    return true;
}

template<typename T>
void MyQueueLinkList<T>::pop_and_get_consumer(T& get_value)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while(empty())
    {
        cv.wait(lock);
    }
    get_value = getFront();
    deQueue();
}

