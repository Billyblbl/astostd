#ifndef GSCOPE
# define GSCOPE

#include <Memory.cpp>
#include <DynArray.cpp>
#include <Object.cpp>

namespace Memory {

	struct Scope : public Generics::DynArrayList<Buffer> {

		inline Buffer	alloc(USize size) {
			return add(buffer.allocator->alloc(size));
		}

		inline Buffer	realloc(USize size, Buffer buff) {
			auto location = find(buff);
			if (location) return *location = buffer.allocator->realloc(size, buff);
			else return Buffer{};
		}

		inline void	dealloc(Buffer buff) {
			return buffer.allocator->dealloc(buff);
		}

		Scope(Allocator& alloc = StandardCAllocator) {
			buffer.allocator = &alloc;
			buffer.count = 0;
			count = 0;
		}

		~Scope() {
			for (auto buff : *this) buffer.allocator->dealloc(buff);
		}

		inline operator Allocator() {
			return MakeScopeAllocator(*this);
		}

		inline operator Allocator&() {
			auto buff = alloc(sizeof(Allocator));
			auto &alloc = Generics::arrayCast<Allocator>(buff)[0];
			alloc = *this;
			return alloc;
		}

	};

	Buffer	ScopeAlloc(Any* memoryContext, USize size) {
		auto scope = (Scope*)memoryContext;
		return scope->alloc(size);
	}

	Buffer	ScopeRealloc(Any* memoryContext, USize size, Buffer buffer) {
		auto scope = (Scope*)memoryContext;
		return scope->realloc(size, buffer);
	}

	void	ScopeDealloc(Any* memoryContext, Buffer buffer) {
		auto scope = (Scope*)memoryContext;
		scope->dealloc(buffer);
	}

	void	ScopeClear(Any* memoryContext) {
		auto scope = (Scope*)memoryContext;
		for (auto buff : *scope)
			scope->buffer.allocator->dealloc(buff);
		scope->clear();
	}

	Allocator	MakeScopeAllocator(Scope& scope) {
		return Allocator {
			&scope, {
				&ScopeAlloc,
				&ScopeRealloc,
				&ScopeDealloc,
				&ScopeClear
			}
		};
	}

} // namespace Memory




#endif