# DnCp
The .NET look-alike threading library for C++

I put this here under the open MIT license to help make this small library shareable accross projects.

The goal of this library is to allow a C++ developer using C++14 or later to save a little time with
threading implementation.  I've always used ACE for large, sophisticated, applications.  But sometimes 
you just need something quick and easy.  That's the point of this library.  It works - doesn't have
threading bugs, etc.

I always find condition variables too verbose compared to the .NET implementation of Events.  So I built that
here - it works the same.

This uses all C++, no add-in library is needed.   

This is just a few headers and nothing else.  So you can just add it to your includes and be done with it.

It's not fancy in any way and suggestions for improvement are always welcomed.

