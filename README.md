# nbtview
A C++ library for fast, read-only access to data in the NBT format (as used by Minecraft)

The library itself is working but its API is subject to change.

A naive search method provides fast access to a tag by not decoding the NBT structure itself.

Beyond that, the standard interface provides the data payloads as elements of either `std::vector` or `std::map`,
modeling NBT List Tags and Compound Tags respectively.

## Root Tag
The root tag is assumed to be an unnamed Compound Tag.

## Tag Payloads
The payloads of tags are represented by a `payload_type`.  The `payload_type` is a `std::variant<...>` that encompasses:
* Single data values (Int, Long, Float, String, etc.) stored directly as `int8_t`, `std::string`, etc.
* Array data values (Byte_Array, etc.) stored as per `unique_ptr<vector<int8_t>>`, etc.
* Compound and List tags stored as per `unique_ptr<Compound_Tag>` and `unique_ptr<List_Tag>`

## Tag Names
The names of any named tags are available as the keys of their parent Compound Tag's data, which is a `std::map<std::string, payload_type>`
