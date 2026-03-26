

/*
	Overview:

	Before going into what the base layer is first I will go into why there are layers.
	Layers are useful in organizing code. A layer is a directory similar to how in odin
	a package is a directory. Layers a textually namespaced, eg a layer named: `os` will
	have everything prefixed by `os` like `os_file_open` with the exception of the base
	layer or user application code. Layers are not allowed to be cyclic eg the base
	layer can't depend on the os layer whilst the os layer depends on the base layer.

	Layers can abstract over platform dependent code by using sub directories that are
	named using the appropriate platform name: `linux`, `windows`, `mac` and so on; the
	same thing applies to files, each file should be suffixed by the platform name eg
	`base_memory_linux.c`, 'os_core_windows.c' etc.

	It is not a requirement that os code be under an `os` layer. For flexibility I decided that
	any layer can have platform dependent code as long as it adheres to the rules above. It
	makes it more annoying to port because the code isn't localized to a single directory, but
	it allows for more flexibility in how the layers can be organized.

	Layers can be nested as child directories of a layer. I refer to these layers as sublayers.

	Layers usually contain a .c and .h file pair.
	The .c file is the implementation and is usually located in the root directory or in
	one of the platform dependent directories. 
	The .h file is extremely important. It acts as a legend or navigation tool for people
	to use to skim around and understand in a broad sense the layer's functionality.

	It is not a requirement to have only one .c or .h file. You can split up functionality
	into multiple files if you want to. Having one .h file is preferred where it is split
	into sections with respect to the organizing of the implementation.

	Base Layer:
	The base layer is designed to be a solid foundation for the codebase by providing
	commonly need functionality up front for other layers to use. All layers downstream
	of the base layer by nature depend on the base layer.

	The base layer should avoid containing any platform dependent code. The exception
	I made here is the memory sublayer as without it, virtual memory backed arenas
	would have a cyclic dependency with a layer outside the base layer.

	The base layer has arenas, strings, math, logging, and other bits of functionality.

	The core data types are:
		u8, u16, u32, u64, u128
		s8, s16, s32, s64
		b8, b16, b32, b64
		f32, f64
		rawptr, cstring
		Slice String
		Arena

	The codebase assumes 64 bit support. It won't compile for 32 bit platforms.

	I default integers to be signed. It's more friendly to use with scenerios like
	reverse for loops, subtracts not wrapping to large numbers, being able to assert
	on negative values so you can catch wrapping bugs etc etc...

	`String` is used for representing strings. These are sized strings as opposed to
	cstrings which are null terminated. Null terminated strings are harder to deal with
	and lead to bugs where non-null terminated strings lead to buffer overruns. So
	instead I do the sensible thing of storing a ptr + length to denote a string.

	All strings are immutable by default, that means you don't write to any of the
	bytes pointed to by a string. This is for two reasons: one is that cstrings are
	immutable so if you write to a cstring you crash and two is that you gain a bit
	of control by having this rule in that you don't lose access to previous memory
	because it was mutated. If `A` can't mutated and you produce `B` from `A`, then
	you have BOTH `B` and `A` without much hassle; you don't have to do gymnastics
	where you copy `A` so that when you mutate it into `B` you have `A` still.

	Memory management across the code base is handled via Arenas. Arenas give an easy
	way to define groups of lifetimes so you can easily do lifetime batching; This way
	is done opposed to the malloc and free naive approach where every single object gets
	a lifetime and you end up in nightmare world where you loop through a list of pointers
	and free each one individually... (don't do that). Instead memory lifetimes should be
	batched eg you could have a `file` arena in the context of a text editor that frees
	everything upon the user closing a file. 
*/
