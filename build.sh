script_path=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
root_path=$script_path
bin_path="$root_path/bin"

mkdir -p "$bin_path"

# --------------------------------------------------------------------------------------------------
# Warnings
disable_unused_funcs="-Wno-unused-function"
disable_unused_vars="-Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-parameter"
disable_no_shadowing="-Wno-shadow"
disable_conditional_uninitialized="-Wno-conditional-uninitialized"
ignored_warnings="-Wno-initializer-overrides -Wno-extra-semi-stmt -Wno-implicit-void-ptr-cast -Wno-cast-qual -Wno-incompatible-pointer-types-discards-qualifiers -Wno-missing-braces -Wno-missing-field-initializers -Wno-pointer-sign -Wno-undef -Wno-pedantic -Wno-reserved-macro-identifier -Wno-declaration-after-statement -Wno-bad-function-cast -Wno-float-equal -Wno-switch-default -Wno-unsafe-buffer-usage -Wno-class-varargs -Wno-cast-align -Wno-disabled-macro-expansion -Wno-padded -Wno-reserved-identifier -Wno-assign-enum -Wno-covered-switch-default"

# --------------------------------------------------------------------------------------------------
# Defines
defines="-DDEBUG=1"
defines="$defines -DBASE_ENTRY_POINT=1"
# defines="$defines -DOS_ENTRY_POINT=1"

# --------------------------------------------------------------------------------------------------
# Options
options="-std=c99 -Weverything -Werror $disable_unused_funcs $disable_no_shadowing $disable_conditional_uninitialized $ignored_warnings"
options="$options $disable_unused_vars"
options="$options -I$root_path"
# options="$options -fsanitize=address"

libs="-lm -lwayland-client -lxkbcommon"
method="build"
mode="scratch"
scratch_package="logger"

# --------------------------------------------------------------------------------------------------
# ctags
ctags --exclude="@$root_path/.ignore_tags" --recurse $root_path

# --------------------------------------------------------------------------------------------------
# Compile

if [ "$mode" == "debug" ]; then
	defines="$defines -DDEBUG=1"
	clang -g -O0 $defines $options $libs $root_path/main.c -o "$bin_path/p"
	if [ "$?" -ne 0 ]; then
		exit 1
	fi
	if [ "$method" == "run" ]; then
		$bin_path/p
		exit 1
	fi
fi

if [ "$mode" == "opt" ]; then
	clang -O3 $defines $options $libs $root_path/main.c -o "$bin_path/p"
	if [ "$?" -ne 0 ]; then
		exit 1
	fi
	if [ "$method" == "run" ]; then
		$bin_path/p
		exit 1
	fi
fi

if [ "$mode" == "scratch" ]; then
	defines="$defines -DDEBUG=1"
	clang -g -O0 $defines $options $libs $root_path/scratch/$scratch_package/main.c -o "$bin_path/p"
	if [ "$?" -ne 0 ]; then
		exit 1
	fi
	if [ "$method" == "run" ]; then
		$bin_path/p
		exit 1
	fi
fi
