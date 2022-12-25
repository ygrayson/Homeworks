#ifndef HASHTABLE_H
#define HASHTABLE_H

// names / uniquenames
// Name: Qianbo Yin
// uniquename: ygrayson

// INSTRUCTIONS:
// fill out the methods in the class below.

// You may assume that the key and value types can be copied and have default
// constructors.

// You can also assume that the key type has (==) and (!=) methods.

// You may assume that Hasher is a functor type with a
// size_t operator()(const Key&) overload.

// The key/value types aren't guaranteed to support any other operations.

// Do not add, remove, or change any of the class's member variables.
// The num_ghosts counter is *optional*. You might find it helpful, but it
// is not required to pass the project.

// Do not change the Bucket type.

// SUBMISSION INSTRUCTIONS:
// Submit this file, by itself, in a .tar.gz.
// Other files will be ignored.

#include <functional> // where std::hash lives
#include <vector>
#include <string>
#include <cassert> // useful for debugging!

// A bucket's status tells you whether it's filled, empty, or contains a ghost.
enum class Status {
    Empty,
    Filled,
    Ghost
};

template<typename K, typename V, typename Hasher = std::hash<K>>
class HashTable {
    // used by the autograder; do not change/remove.
    friend class Verifier;
public:
    // A bucket has a status, a key, and a value.
    struct Bucket {
        // Do not modify Bucket.
        Status status = Status::Empty;
        K key;
        V val;
    };

    HashTable() {
        //DONE: a default constructor (possibly empty).

        // You can use the following to avoid needing to implement rehashing.
        // However, you will only pass the AG test cases ending in _C.
        // To pass the rest, start with at most 20 buckets and implement rehashing.
        buckets.resize(20);
    }

    size_t size() const {
        return num_elements;
    }

    // returns a reference to the value in the bucket with the key, if it
    // already exists. Otherwise, insert it with a default value, and return
    // a reference to the resulting bucket.
    V& operator[](const K& key) {
        //DONE
        size_t idx = find(key);
		//does not find the key
        if(idx == std::string::npos) {
            insert(key, V());
            return buckets[find(key)].val;
        }
        else {
            return buckets[idx].val;
        }
    }

    // insert returns whether inserted successfully
    // (if the key already exists in the table, do nothing and return false).
    bool insert(const K& key, const V& val) {
        //DONE
        //key already in the table
        if(find(key) != std::string::npos)
            return false;
        
        Hasher hasher;
        size_t desired_idx = hasher(key) % buckets.size();
        while(buckets[desired_idx].status == Status::Filled) {
            ++desired_idx;
			desired_idx = desired_idx % buckets.size();
        }
        //insert at the first Empty/Ghost place
		if (buckets[desired_idx].status == Status::Ghost)
			--num_ghosts;
		buckets[desired_idx].status = Status::Filled;
        buckets[desired_idx].key = key;
        buckets[desired_idx].val = val;

        ++num_elements;
        if((double) buckets.size() * 0.4 < (double) num_elements || 
			(double)buckets.size() * 0.9 < (double)(num_elements + num_ghosts))
            rehash_and_grow();
        return true;
    }

    // erase returns the number of items remove (0 or 1)
    size_t erase(const K& key) {
        //DONE
        size_t idx = find(key);
        if(idx == std::string::npos)
            return 0;
        else {
            buckets[idx].status = Status::Ghost;
			--num_elements;
			++num_ghosts;
			
            return 1;
        }
    }

private:
    size_t num_elements = 0;
    size_t num_ghosts = 0; // OPTIONAL: you don't need to use this to pass
    std::vector<Bucket> buckets;

    void rehash_and_grow() {
        //DONE
        buckets.resize(buckets.size() * 2);
        for(size_t i = 0; i < buckets.size(); ++i) {
            switch(buckets[i].status) {
            case (Status::Empty):
                break;
            case (Status::Ghost):
                buckets[i].status = Status::Empty;
                break;
            case (Status::Filled):
                buckets[i].status = Status::Empty;
                --num_elements;
                insert(buckets[i].key, buckets[i].val);
            }
        }
		num_ghosts = 0;
    }

    // You can add methods here if you like.
    // DONE

    //find()
    size_t find(const K &key) {
        Hasher hasher;
        size_t idx = hasher(key) % buckets.size();

        //at the hashed idx, linear probing search
        while(buckets[idx].status != Status::Empty) {
            if(buckets[idx].status == Status::Filled && buckets[idx].key == key)
                break;
            ++idx;
			idx = idx % buckets.size();
        }

        //return according to find/not
        if(buckets[idx].status == Status::Empty)
            return std::string::npos;
        else
            return idx;
    }
};

#endif // HASHTABLE_H