#pragma once
#include "MemoryTracker.h"
#include "LayerContainers.h"


template<typename functionSig>
class Broadcaster
{
private:
	struct FunctionRef
	{
		FunctionRef(void* pObj_in, std::function<functionSig> function_in) 
			: pObj(pObj_in), functionCall(function_in) {}
		
		void* pObj = nullptr;
		std::function<functionSig> functionCall;
	};

public:
	// Add function to list to be called (once) when broadcaster broadcasts.
	// Any Register() call on a object's method should call Unregister() in it's destructor or sometime before it's destroyed.
	// -pContextObject: Pointer to relevant context object that will be used for the function. Usually 'this' for member functions, and 'nullptr' for global or static functions.
	// -functionToBeCalled: Can call function by wrapping it in a std::bind or a lambda.
	// 
	// >No parameter static function:		
	//	myBroadcaster.Register(nullptr, MyFunction);
	// 
	// >No parameter std::bind member function:		
	//	myBroadcaster.Register(nullptr, std::bind(&MyClass::MyFunc, this);
	// 
	// >1 parameter std::bind static function:	
	//	myBroadcaster.Register(nullptr, std::bind(&MyFunc, std::placeholders::_1));
	// 
	// >1 parameter std::bind member function:	
	//	myBroadcaster.Register(this, std::bind(&MyClass::MyFunc, this, std::placeholders::_1));
	// 
	// >No parameter Lambda member function:	
	//	myBroadcaster.Register(this, [this](){ this->MyFunc() });
	// 
	// >1 parameter Lambda member function:	
	//	myBroadcaster.Register(this, [this](paramType param){ this->MyFunc(param) });
	// 
	// >1 parameter Lambda static function:
	//	myBroadcaster.Register(nullptr, [](paramType param){ MyFunc(param) });
	void Register(void* pContextObject, std::function<functionSig> functionToBeCalled)
	{
		m_FunctionsToBeCalled.emplace_back(pContextObject, functionToBeCalled);
	}

	// Call every method registered to this broadcaster. 
	// If the functions require parameters to be passed in then that would be done here.
	template<typename... Args>
	void Broadcast(Args&... args)
	{
		for (const auto& funcRef : m_FunctionsToBeCalled)
		{
			funcRef.functionCall(args...);
		}
	}

	// Use context object, usually 'this', to remove all relevant object functions from the function call array. 
	// WARNING: If 'nullptr' is passed in then ALL functions that used a nullptr context will be removed from the function call array,
	// this includes all the Globals that are register with the context 'nullptr'.
	// Any Register() call on a object's method should call Unregister() in it's destructor or sometime before it's destroyed.
	void Unregister(void* pContextObject)
	{
		auto new_end = std::remove_if(m_FunctionsToBeCalled.begin(), m_FunctionsToBeCalled.end(),
			[pContextObject](const FunctionRef& funcRef) {
				return funcRef.pObj == pContextObject;
			});
		m_FunctionsToBeCalled.erase(new_end, m_FunctionsToBeCalled.end());
	}

private:
	T_vector<FunctionRef, MT_ENGINE> m_FunctionsToBeCalled = {};
};

