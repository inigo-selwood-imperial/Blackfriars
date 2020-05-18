#pragma once

typedef std::size_t Hash;

Hash combine(const Hash &hash_one, const Hash &hash_two) {
    return hash_one ^
            (hash_two + 0x9e3779b9 + (hash_one << 6) + (hash_one >> 2));
}

Hash combine(std::vector<Hash> hashes) {
    if(hashes.empty())
        return 0;

    Hash result = hashes.back();
    hashes.pop_back();

    while(hashes.empty() == false) {
        result = combine(result, hashes.back());
        hashes.pop_back();
    }

    return result;
}
