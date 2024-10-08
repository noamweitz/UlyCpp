#include "../include/arrays/ast_array_access.hpp"

std::string ArrayAccess::GetIdentifier() const
{
    Identifier *identifier = dynamic_cast<Identifier *>(identifier_);
    ArrayAccess *array_access = dynamic_cast<ArrayAccess *>(identifier_);

    if (identifier != nullptr)
    {
        return identifier->GetIdentifier();
    }
    else if (array_access != nullptr)
    {
        return array_access->GetIdentifier();
    }

    throw std::runtime_error("ArrayAccess GetIdentifier: Identifier not found");
}

void ArrayAccess::EmitRISC(std::ostream &stream, Context &context, std::string passed_reg) const
{
    // Get variable specifications
    Variable variable_specs = context.get_variable(GetIdentifier());

    // Get type of atomic element
    Type type = IsPointerOperation(context) ? Type::_INT : GetType(context);

    // Get index of specific element
    std::string index_register = context.get_register(Type::_INT);
    GetIndex(stream, context, index_register, type);

    // Fetch element from variable-binding specified memory location if local scope
    if (variable_specs.scope == Scope::_LOCAL)
    {
        if (variable_specs.is_pointer)
        {
            // Pointers points to first item in list
            std::string pointer_register = context.get_register(Type::_INT);
            stream << context.load_instruction(Type::_INT) << " " << pointer_register << ", " << variable_specs.offset << "(s0)" << std::endl;
            stream << "add " << index_register << ", " << index_register << ", " << pointer_register << std::endl;
            context.deallocate_register(pointer_register);
        }
        else if (variable_specs.is_array)
        {
            // Add index to base pointe
            stream << "add " << index_register << ", " << index_register << ", s0" << std::endl;
            stream << "addi " << index_register << ", " << index_register << ", " << variable_specs.offset << std::endl;
        }
        else
        {
            throw std::runtime_error("ArrayAccess EmitRISC: Variable is not a pointer or array");
        }

        stream << context.load_instruction(type) << " " << passed_reg << ", 0(" << index_register << ")" << std::endl;
    }

    // Fetch element from label-specified memory location if global scope
    else if (variable_specs.scope == Scope::_GLOBAL)
    {
        std::string global_memory_location = "global_" + GetIdentifier();
        std::string global_memory_register = context.get_register(Type::_INT);

        stream << "lui " << global_memory_register << ", " << "%hi(" << global_memory_location << ")" << std::endl;
        stream << "add " << global_memory_register << ", " << global_memory_register << ", " << index_register << std::endl;
        stream << context.load_instruction(type) << " " << passed_reg << ", %lo(" << global_memory_location << ")(" << global_memory_register << ")" << std::endl;

        context.deallocate_register(global_memory_register);
    }

    // Deallocate register
    context.deallocate_register(index_register);
}

void ArrayAccess::GetIndex(std::ostream &stream, Context &context, std::string passed_reg, Type type) const
{
    // Set operation type as dealing with pointers
    context.set_operation_type(Type::_INT);

    // Emit index to specified register
    index_->EmitRISC(stream, context, passed_reg);

    // Shift index by log2(size) to get byte offset
    stream << "slli " << passed_reg << ", " << passed_reg << ", " << types_shift.at(type) << std::endl;

    context.pop_operation_type();
}

void ArrayAccess::Print(std::ostream &stream) const
{
    identifier_->Print(stream);
    stream << "[";
    index_->Print(stream);
    stream << "]";
}

Type ArrayAccess::GetType(Context &context) const
{
    Variable variable_specs = context.get_variable(GetIdentifier());
    return variable_specs.type;
}

bool ArrayAccess::IsPointerOperation(Context &context) const
{
    Variable variable_specs = context.get_variable(GetIdentifier());

    if (!variable_specs.is_pointer)
    {
        return false;
    }

    return variable_specs.dereferences_number > 1;
}
