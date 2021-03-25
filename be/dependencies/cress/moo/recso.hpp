/*
//	Bachelor of Software Engineering
//	Media Design School
//	Auckland
//	New Zealand
//
//	(c) 2019 Media Design School
//
//	File		:	cress/moo/recso.hpp
//
//	Summary		:	Reference Counted Static Object.
//					The concept can be related to the Singleton pattern.
//
//	Project		:	moo
//	Namespace	:	cress::moo
//	Author		:	Elijah Shadbolt
//	Email		:	elijah.sha7979@mediadesign.school.nz
//	Date Edited	:	23/10/2019
*/

#pragma once

#include <memory>
#include <mutex>
#include <atomic>

#ifndef CRESS_MOO_NO_CASSERT
#include <cassert>
#endif



namespace cress
{
	namespace moo
	{
		/*
		//	Recso
		//	Reference Counted Static Object.
		//	The concept can be related to the Singleton pattern.
		//
		//	This pointer-like object shares ownership of statically allocated storage with all other instances of this concrete type.
		//	The shared value always exists (is never null) if at least one instance of the Resco class exists.
		//	The type parameter |Traits| is the tag that determines which shared variable is used by the instance.
		//	Recso is thread safe for reference counting.
		//	The dereferenced value is not inherently thread safe.
		//
		//	The |Traits| type parameter must have the following members:
		//
		//		type Value
		//			Type of the value that comes from dereferencing the shared storage.
		//
		//		type Storage
		//			Type of the static shared variable.
		//			For example, |Value*|.
		//
		//		type Pointer
		//			For example, |Value*|.
		//
		//		type ConstPointer
		//			For example, |Value const*|.
		//
		//		type Reference
		//			For example, |Value&|.
		//
		//		type ConstReference
		//			For example, |Value const&|.
		//
		//		static Pointer address(Storage& storage);
		//			For example, if Storage is Value*, returns |storage|.
		//
		//		static ConstPointer addressConst(Storage const& storage);
		//			For example, if Storage is Value*, returns |storage|.
		//
		//		static Reference deref(Storage& storage);
		//			For example, if Storage is Value*, returns |*storage|.
		//
		//		static ConstReference derefConst(Storage const& storage);
		//			For example, if Storage is Value*, returns |*storage|.
		//
		//		static void destroy(Storage const& storage) noexcept;
		//			This is called when the last reference is destructed.
		//			For example, if Storage is Value*, |delete storage|.
		//
		//		static void create(Storage& storage, Args...);
		//			This is called when the first reference is constructed.
		//			It can have any number of Args after the storage.
		//			The args are forwarded from the constructor call of the Recso that is the first reference.
		//			The Recso constructor overload set is a mirror of the Traits::create overload set.
		//			For example, if Storage is Value*, |storage = new Value(args...)|.
		//
		//		static void require(bool condition) noexcept;
		//			This is called in the event of an unrecoverable error:
		//			the reference count is about to overflow max or underflow negative.
		*/
		template<class Traits_>
		class Recso
		{
		public:
			using This = Recso;
			using Traits = Traits_;

			using Storage = typename Traits_::Storage;
			using Value = typename Traits_::Value;
			using Pointer = typename Traits_::Pointer;
			using ConstPointer = typename Traits_::ConstPointer;
			using Reference = typename Traits_::Reference;
			using ConstReference = typename Traits_::ConstReference;

		private:
			static Pointer s_address()
			{
				return Traits_::address(static_cast<Storage&>(storage));
			}
			static ConstPointer s_addressConst()
			{
				return Traits_::addressConst(static_cast<Storage const&>(storage));
			}
			static Reference s_deref()
			{
				return Traits_::deref(static_cast<Storage&>(storage));
			}
			static ConstReference s_derefConst()
			{
				return Traits_::derefConst(static_cast<Storage const&>(storage));
			}

			inline static Storage storage;
			inline static std::atomic_size_t refcount = 0;
			inline static std::mutex mutex;

		public:
			using Count = typename decltype(refcount)::value_type;

		private:
			static void incrementRefCount() noexcept
			{
				Traits_::require(refcount != std::numeric_limits<Count>::max());
				++refcount;
			}

		public:
			~Recso() noexcept
			{
				std::scoped_lock lock{ mutex };
				Traits_::require(refcount > static_cast<Count>(0));
				--refcount;
				if (static_cast<Count>(0) == refcount)
				{
					Traits_::destroy(storage);
				}
			}

			Recso(This const&) noexcept { incrementRefCount(); }
			Recso(This&&) noexcept { incrementRefCount(); }

			// does not change ref count
			This& operator=(This const&) noexcept = default;
			This& operator=(This&&) noexcept = default;
			void swap(This&) noexcept {}
			friend void swap(This&, This&) noexcept {}

			/*
			//	Forwards arguments to |Traits::create(storage, args...)|
			//	if this is the first reference that was created.
			*/
			template<class...Args>
			explicit Recso(Args&& ...args)
			{
				std::scoped_lock lock{ mutex };
				if (static_cast<Count>(0) == refcount)
				{
					Traits_::create(storage, std::forward<Args>(args)...);
				}
				incrementRefCount();
			}

			Recso()
			{
				std::scoped_lock lock{ mutex };
				if (static_cast<Count>(0) == refcount)
				{
					Traits_::create(storage);
				}
				incrementRefCount();
			}

			// refcount
			static Count refCount() noexcept { return refcount.load(); }

			// address
			Pointer address() { return s_address(); }
			ConstPointer address() const { return s_addressConst(); }
			ConstPointer addressConst() const { return s_addressConst(); }

			Pointer operator->() { return s_address(); }
			ConstPointer operator->() const { return s_addressConst(); }

			// deref
			Reference deref() { return s_deref(); }
			ConstReference deref() const { return s_derefConst(); }
			Reference derefConst() const { return s_derefConst(); }

			Reference operator*() { return s_deref(); }
			ConstReference operator*() const { return s_derefConst(); }
		};



		/*
		//	Base class of an example user-defined Traits type which is used as the type parameter for Rcso<Traits>.
		//	Allocates and deallocates the Value in aligned storage directly in the static Rcso variable.
		//
		//	The derived class must implement |static void create(Storage& storage, Args...)|
		//	and have it call |createImpl(storage, args...)|.
		//
		//	WARNING: You should not use this type as the Traits argument without deriving from it,
		//	because all other |Recso<StaticAllocRecsoTraits<Value>>| with the same Value type will use the same storage.
		//
		//	Example:

			struct MyTraits : public cress::moo::StaticRecsoTraits<std::string>
			{
				using Super = cress::moo::StaticRecsoTraits<std::string>;
				static void create(typename Super::Storage& storage)
				{
					// constructs std::string(char const*) in place.
					Super::createWithArguments(storage, "my data");
				}
			};
			using MyRecso = cress::moo::Recso<MyTraits>;

			MyRecso a;
			std::cout << *a << '\n'; // prints "my data".
			MyRecso b;
			*b = "hello";
			std::cout << *a << '\n'; // prints "hello".

		*/
		template<class Value_>
		struct StaticAllocRecsoTraits
		{
			using This = StaticAllocRecsoTraits;
			using Value = Value_;

			using Storage = std::aligned_storage_t<sizeof(Value), alignof(Value)>;
			using Pointer = Value *;
			using ConstPointer = Value const*;
			using Reference = Value &;
			using ConstReference = Value const&;



			static Pointer address(Storage& storage) noexcept
			{
				return reinterpret_cast<Value_*>(std::addressof(storage));
			}

			static ConstPointer addressConst(Storage const& storage) noexcept
			{
				return reinterpret_cast<Value_ const*>(std::addressof(storage));
			}

			static Reference deref(Storage& storage) noexcept
			{
				return *address(storage);
			}

			static ConstReference derefConst(Storage const& storage) noexcept
			{
				return *addressConst(storage);
			}

			static void destroy(Storage& storage) noexcept
			{
				reinterpret_cast<Value_*>(std::addressof(storage))->~Value_();
			}

			/*
			//	This is named |createWithArguments| to prevent unwanted overloading of |Traits::create|.
			*/
			template<class...Args>
			static void createWithArguments(Storage& storage, Args&& ...args)
				noexcept(std::is_nothrow_constructible_v<Value_, Args&&...>)
			{
				new (std::addressof(storage)) Value_(std::forward<Args>(args)...);
			}

			/*
			//	This overload set can be hidden by a derived class for custom behaviour.
			*/
			static void create(Storage& storage)
				noexcept(std::is_nothrow_default_constructible_v<Value_>)
			{
				createWithArguments(storage);
			}

			static void require(bool condition) noexcept
			{
#ifndef CRESS_MOO_NO_CASSERT
				assert(condition);
#endif
				if (!condition) { std::terminate(); }
			}
		};

		/*
		//	Recso with static allocation that calls Value's default constructor.
		*/
		template<class Value_>
		using StaticAllocRecso = Recso<StaticAllocRecsoTraits<Value_>>;



		template<class Value_>
		struct HeapAllocRecsoTraits
		{
			using This = HeapAllocRecsoTraits;
			using Value = Value_;

			using Storage = Value *;
			using Pointer = Value *;
			using ConstPointer = Value const*;
			using Reference = Value &;
			using ConstReference = Value const&;



			static Pointer address(Storage const storage) noexcept
			{
				return storage;
			}

			static ConstPointer addressConst(Storage const storage) noexcept
			{
				return storage;
			}

			static Reference deref(Storage const storage) noexcept
			{
				return *storage;
			}

			static ConstReference derefConst(Storage const storage) noexcept
			{
				return *storage;
			}

			static void destroy(Storage const storage) noexcept
			{
				delete storage;
			}

			template<class...Args>
			static void createWithArguments(Storage& storage, Args&& ...args)
				noexcept(false) // new may throw
			{
				storage = new Value_(std::forward<Args>(args)...);
			}

			/*
			//	This overload set can be hidden by a derived class for custom behaviour.
			*/
			static void create(Storage& storage)
				noexcept(false) // new may throw
			{
				createWithArguments(storage);
			}

			static void require(bool condition) noexcept
			{
#ifndef CRESS_MOO_NO_CASSERT
				assert(condition);
#endif
				if (!condition) { std::terminate(); }
			}
		};

		/*
		//	Recso with heap allocation that calls Value's default constructor.
		*/
		template<class Value_>
		using HeapAllocRecso = Recso<HeapAllocRecsoTraits<Value_>>;



		/*
		//	Singleton
		//	Use this when you know that it will be alive for the entire lifetime of the application.
		*/
		template<class Traits_>
		class Singleton final
		{
		public:
			using This = Singleton;
			using Traits = Traits_;

			using Storage = typename Traits_::Storage;
			using Value = typename Traits_::Value;
			using Pointer = typename Traits_::Pointer;
			using ConstPointer = typename Traits_::ConstPointer;
			using Reference = typename Traits_::Reference;
			using ConstReference = typename Traits_::ConstReference;
		private:
			inline static Storage s_storage;
		public:
			~Singleton() = delete;
			Singleton() = delete;
			Singleton(Singleton const&) = delete;
			Singleton(Singleton&&) = delete;
			Singleton& operator=(Singleton const&) = delete;
			Singleton& operator=(Singleton&&) = delete;

			template<class...Args>
			static Reference create(Args&&...args)
			{
				Traits_::create(s_storage, std::forward<Args>(args)...);
				return Traits_::deref(s_storage);
			}
			static Reference get() { return Traits_::deref(s_storage); }
			static ConstReference cget() { return Traits_::derefConst(s_storage); }
			static void destroy() noexcept { Traits_::destroy(s_storage); }

			// KEEPALIVE
			struct Deleter {
				using pointer = This::Pointer;
				void operator()(pointer p) noexcept {
					if (p) { Singleton::destroy(); }
				}
			};
			using UniquePtr = std::unique_ptr<Value, Deleter>;
			template<class...Args>
			static UniquePtr make_unique(Args&&...args)
			{
				This::create(std::forward<Args>(args)...);
				return UniquePtr(Traits_::address(s_storage));
			}

			using SharedPtr = std::shared_ptr<Value>;
			template<class...Args>
			static SharedPtr make_shared(Args&&...args)
			{
				This::create(std::forward<Args>(args)...);
				return SharedPtr(Traits_::address(s_storage), Deleter());
			}
			// NOTE: weak_ptr should never be used for these singletons
			// because they are expected to always be alive when used.
		};
	}
}
