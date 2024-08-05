#include "../include/arrays/ast_array_access.hpp"

std::string ArrayAccess::GetIdentifier() const
{
    return dynamic_cast<Identifier *>(identifier_)->GetIdentifier();
}

void ArrayAccess::EmitRISC(std::ostream &stream, Context &context, std::string passed_reg) const
{
    // Get variable specifications
    Variable variable_specs = context.get_variable(GetIdentifier());

    // Get type of atomic element
    Type type = GetType(context);

    // Get index of specific element
    std::string index_register = context.get_register(Type::_INT);
    GetIndex(stream, context, index_register, type);

    // Get variable offset
    stream << context.load_instruction(type) << " " << passed_reg << ", " << variable_specs.offset << "(" << index_register << ")" << std::endl;

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

    // Add index to base pointer
    stream << "add " << passed_reg << ", " << passed_reg << ", sp" << std::endl;

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
