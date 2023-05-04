# nbtview
A C++ library for fast, read-only access to data in the NBT format (as used by Minecraft)

First of all, the library is a work in progress, and doesn't do anything yet.

But the basic concept is to provide a view of the data in the NBT structure.
The structure will be read over to the extent of creating an object for each tag, but the tag payload in
the case of data tags (by which I mean tags other than List and Compound) will not be copied.  This will avoid
a significant amount of heap allocation.  An important tradeoff is that the view is only valid so long as the data
it was produced for remains alive and unchanged.
