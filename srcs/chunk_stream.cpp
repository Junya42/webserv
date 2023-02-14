#include "../includes/client.hpp"

int Request::state_0(std::istringstream &stream) {

    char c;

    chunk_size = 0;
    stream.get(c);
    if (stream.gcount() == 0) {
        return 0;
    }
    if (!(isalnum(c) || c == '\r' || c == '\n')) {

        set_error(400);
        fclose(cfile);
        complete_chunk = true;
        state = 3;
        return -1;
    }
    if (to_skip) {
        
        to_skip--;
        return 0;
    }
    temp0 += c;
    if (temp0.size() > 2 && temp0[temp0.size() - 1] == '\n' && temp0[temp0.size() - 2] == '\r') {

        sscanf(temp0.c_str(), "%x", &chunk_size);
        temp0.clear();
        if (chunk_size) {

            state = 1;
            return 1;
        }
        else {

            state = 2;
            return 2;
        }
    }
    return 0;
}

int Request::state_1(std::istringstream &stream) {
    
    std::vector<unsigned char> buff(buff_size);
    std::string tmp;
    int int_bytes;
    
    if (chunk_left) {

        chunk_size = chunk_left;
        chunk_left = 0;
    }

    stream.read(reinterpret_cast<char *>(&buff[0]), chunk_size);
    int_bytes = stream.gcount();
    for (int i = 0; i < int_bytes; i++) {

        tmp.push_back(buff[i]);
    }

    if (tmp.size())
        fwrite(tmp.c_str(), 1, tmp.size(), cfile);
    if (int_bytes == chunk_size) {
        
        to_skip = 2;
        state = 0;
        return 0;
    }
    chunk_left = chunk_size - int_bytes;
    return -1;
}

int Request::state_2(std::istringstream &stream) {
    
    std::vector<unsigned char> buff(15);
    int int_bytes;
    
    stream.read(reinterpret_cast<char *>(&buff[0]), 10);
    int_bytes = stream.gcount();
    if (int_bytes > 5) {

        fclose(cfile);
        set_error(400);
        complete_chunk = true;
        state = 3;
        return -1;
    }

    for (int i = 0; i < int_bytes; i++) {

        temp2.push_back(buff[i]);
    }

    if (temp2.size() == 2 && temp2 == "\r\n") {
        fclose(cfile);
        complete_chunk = true;
        state = 3;
        return -1;
    }
    else if (temp2.size() > 5) {
        fclose(cfile);
        set_error(400);
        complete_chunk = true;
        state = 3;
        return -1;
    }
    return -1;
}

std::string generate_random_filename(void) {
    std::string alpha = "abcdefghijklmnopqrstuvwxyz";
    std::string num = "0123456789_";
    
    std::random_shuffle(alpha.begin(), alpha.end());
    std::random_shuffle(num.begin(), num.end());

    alpha += num;
    std::random_shuffle(alpha.begin(), alpha.end());

    return alpha;
}

void    Request::state_func(std::istringstream &stream) {
    
    cname = generate_random_filename();
    cfile = fopen(cname.c_str(), "w+b");
    
    if (!cfile) {
        
        set_error(500);
        complete_chunk = true;
        return ;
    }

    while (!stream.eof()) {

        switch (state) {
            
            case 0:
                if (state_0(stream) < 0)
                    return ;
                break ;
            case 1:
                if (state_1(stream) < 0)
                    return ;
                break ;
            case 2:
                if (state_2(stream) < 0)
                    return ;
                break ;
            default:
                return ;
        }
    }
    PRINT_LOG("OUT OF WHILE");
}