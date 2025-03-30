#ifndef _WIFI_CONNECTION_H
#define _WIFI_CONNECTION_H

#include <lwip/tcp.h>
#include <cstring>
#include <DCCStream.h>
#include <stdarg.h>

class TCPSocketStream : public DCCExController::DCCStream {
private:
    struct tcp_pcb *pcb;
    struct pbuf *recv_buffer;

    static err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
        TCPSocketStream *stream = static_cast<TCPSocketStream*>(arg);
        if (p == nullptr) {
            // Connection closed
            tcp_close(tpcb);
            stream->pcb = nullptr;
            return ERR_OK;
        }
        if (err == ERR_OK) {
            if (stream->recv_buffer == nullptr) {
                stream->recv_buffer = p;
            } else {
                pbuf_chain(stream->recv_buffer, p); // Safely chain the new buffer
            }
        } else {
            pbuf_free(p);
        }
        return ERR_OK;
    }

public:
    explicit TCPSocketStream(struct tcp_pcb *pcb) : pcb(pcb), recv_buffer(nullptr) {
        tcp_recv(pcb, recv_callback);
        tcp_arg(pcb, this);
    }

    // Check if data is available to read
    int available() const {
        return recv_buffer ? recv_buffer->tot_len : 0;
    }

    // Read a single byte from the socket
    int read() {
        if (recv_buffer == nullptr) {
            return -1; // No data
        }
        uint8_t byte = *static_cast<uint8_t*>(recv_buffer->payload);
        pbuf_remove_header(recv_buffer, 1);
        if (recv_buffer->len == 0) {
            printf("Freeing buffer %d %d %d\n", recv_buffer->len, recv_buffer->tot_len, recv_buffer->ref);
             fflush(stdout);
            struct pbuf *next = recv_buffer->next;
            pbuf_free(recv_buffer);
            recv_buffer = next;
            printf("Buffer freed\n");
             fflush(stdout);
        }
        return byte;
    }

    // Write a buffer to the socket
    size_t write(const uint8_t *buffer, size_t size) {
        err_t err = tcp_write(pcb, buffer, size, TCP_WRITE_FLAG_COPY);
        if (err == ERR_OK) {
            tcp_output(pcb);
            return size;
        }
        return 0; // Error
    }

    // No-op for sockets (no explicit flushing needed)
    void flush() {}

    // Send a string with a newline
    void println(const char* format, ...){
        char buffer[256];
        
        va_list args;     // Declare the variable argument list

        // Initialize the variable argument list
        va_start(args, format);

        // Use vsnprintf to format the string into the buffer
        vsnprintf(buffer, sizeof(buffer), format, args);

        // End the variable argument list
        va_end(args);


        write(reinterpret_cast<const uint8_t*>(buffer), strlen(buffer));
        write(reinterpret_cast<const uint8_t*>("\n"), 1);
    }

    // Send a string
    void print(const char* format, ...) {
        char buffer[256];
        
        va_list args;     // Declare the variable argument list

        // Initialize the variable argument list
        va_start(args, format);

        // Use vsnprintf to format the string into the buffer
        vsnprintf(buffer, sizeof(buffer), format, args);

        // End the variable argument list
        va_end(args);


        write(reinterpret_cast<const uint8_t*>(buffer), strlen(buffer));
    }

    // Destructor to close the socket
    ~TCPSocketStream() {
        if (pcb != nullptr) {
            tcp_close(pcb);
        }
        if (recv_buffer != nullptr) {
            pbuf_free(recv_buffer);
        }
    }
};

#endif