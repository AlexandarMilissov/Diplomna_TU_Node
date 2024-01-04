#ifndef PAYLOAD_HPP_
#define PAYLOAD_HPP_

#include "Common.hpp"

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
    public:
        /**
         * @brief The data of the payload.
         */
        uint8* data;

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
        Payload(const uint8*, const size_t);

        /**
         * @brief Construct a new Payload object.
         *
         * @param mac The MAC address of the payload.
         */
        Payload(const MacAddress&);

        /**
         * @brief Destructor for the Payload object.
        */
        ~Payload();

        /**
         * @brief Overloaded += operator for concatenating two Payload objects.
         *
         * This operator appends the data of the given Payload object to the current Payload object.
         * The size of the current Payload object is increased accordingly.
         *
         * @param add The Payload object to be appended.
         * @return Reference to the modified Payload object.
         */
        Payload& operator+=(const Payload&);

        /**
         * @brief Overloaded right shift assignment operator for extracting a sub-payload from the current payload.
         *
         * This operator allows extracting a sub-payload from the current payload by modifying the current payload and returning a reference to it.
         * The sub-payload is specified as the argument to the operator.
         *
         * @param sub The sub-payload to be extracted from the current payload.
         * @return A reference to the modified current payload.
         * @throws std::invalid_argument if the size of the sub-payload is greater than the size of the current payload.
         */
        Payload& operator>>=(Payload&);

        /**
         * @brief Get the size of the payload.
         *
         * @return The size of the payload.
         */
        size_t GetSize() const;
};

#endif // PAYLOAD_HPP_
