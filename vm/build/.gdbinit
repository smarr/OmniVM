# A simple .gdbinit script to improve the low-level debuggability of the RoarVM

# Standard helper

define print_all_stack_traces
    call pat()
end
document print_all_stack_traces
|   Prints all Smalltalk processes' stacks
end
#alias -a pat = print_all_stack_traces

define print_stack_trace
    call pst()
end
document print_stack_trace
|   Prints the stack trace of the current thread's interpreter instance.
end
#alias -a pst = print_stack_trace

define print_stack_frame
    call print_stack_frame()
end
document print_stack_frame
|   Prints the current top most Smalltalk frame
end
#alias -a psf = print_stack_frame

define print_object
    call dp($arg0)
end
document print_object
|   Prints a Smalltalk object
end
#alias -a dp = print_object

define print_object_with_fields
    call dpf($arg0)
end
document print_object_with_fields
|   Prints a Smalltalk object and its fields
end
#alias -a dpf = print_object_with_fields

define enable_tracing
    call enableTracing()
end
document enable_tracing
|   Starts to print out all executed bytecodes
end

define disable_tracing
    call disableTracing()
end
document disable_tracing
|   Stop to print all executed bytecodes
end

define disable_context_switches
    call disable_context_switches()
end
document disable_context_switches
|   Stop checking for interrupts and only reschedule contexts when explicitly asked for it.
end

define reenable_context_switches
    call reenable_context_switches()
end
document reenable_context_switches
|   Start checking for interrupts again. Will allow rescheduling of contexts.
end


# _____ as long as apple does not support aliases in GDB ________

define pat
    call pat()
end
document pat
|   Prints all Smalltalk processes' stacks
end

define pst
    call pst()
end
document pst
|   Prints the stack trace of the current thread's interpreter instance.
end

define psf
    call print_stack_frame()
end
document psf
|   Prints the current top most Smalltalk frame
end

define dp
    call dp($arg0)
end
document dp
|   Prints a Smalltalk object
end

define dpf
    call dpf($arg0)
end
document dpf
|   Prints a Smalltalk object and its fields
end


define dis_cs
    call disable_context_switches()
end
document dis_cs
|   Stop checking for interrupts and only reschedule contexts when explicitly asked for it.
end

define en_cs
    call reenable_context_switches()
end
document en_cs
|   Start checking for interrupts again. Will allow rescheduling of contexts.
end

