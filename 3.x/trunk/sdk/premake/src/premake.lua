package.name     = "Premake"
package.language = "c"
package.kind     = "exe"
package.target   = "premake"

-- Build Flags

	package.buildflags = 
	{ 
		"no-64bit-checks",
		"static-runtime",
		"extra-warnings"
	}

	package.config["Release"].buildflags = 
	{ 
		"no-symbols", 
		"optimize-size",
		"no-frame-pointers"
	}


-- Defines

	package.defines = {
		"_CRT_SECURE_NO_DEPRECATE",     -- to avoid VS2005 stdlib warnings
	}

	if (target == "gnu") then           -- to get Lua popen support under OS X
		table.insert(package.defines, "USE_POPEN=1")   
	end
	

-- Libraries

	if (OS == "linux") then
		package.links = { "m" }
	end


-- Files

	package.files =
	{
		matchrecursive("*.h", "*.c")
	}
