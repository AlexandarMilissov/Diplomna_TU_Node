#ifndef PAYLOAD_HPP_
#define PAYLOAD_HPP_

#include "Common.hpp"
#include <stack>
#include <queue>
#include <string>

#include "Messages.hpp"

class MacAddress;

/**
 * @brief The Payload class represents a data payload.
 *
 * This class encapsulates a data payload along with its size.
 * It provides various operations for manipulating and accessing the payload data.
 */
class Payload
{
    private:
        /**
         * @brief The size of the payload.
         */
        size_t size;

        /**
         * @brief The data of the payload.
         */
        void* data;
    public:

        /**
         * @brief Construct a new Payload object.
         *
         * @param payload The payload to copy.
         */
        Payload(const Payload&);

        /**
         * @brief Construct a new Payload object.
         *
         * @param size The size of the payload.
         */
        Payload(const size_t);

        /**
         * @brief Construct a new Payload object.
         *
         * @param data The data of the payload.
         * @param size The size of the payload.
         */
        Payload(const void*, const size_t);

        /**
         * @brief Construct a new Payload object.
         *
         * @param mac The MAC address of the payload.
         */
        Payload(const MacAddress&);

        /**
         * @brief Constructs a Payload object with the given string.
         *
         * @param str The string to be stored in the Payload object.
         */
        Payload(const std::string);

        /**
         * @brief Constructs a Payload object with the given message.
         *
         * @param message The message to be stored in the Payload.
         */
        Payload(MessageType);

        /**
         * @brief Destructor for the Payload object.
        */
        ~Payload();

        /**
         * @brief Get the size of the payload.
         *
         * @return The size of the payload.
         */
        size_t GetSize() const;

        void* GetData() const;

        static Payload Compose(std::stack<Payload>);

        static std::queue<Payload> Decompose(const Payload&);
        static std::queue<Payload> Decompose(const void*, const size_t);
};

#endif // PAYLOAD_HPP_
