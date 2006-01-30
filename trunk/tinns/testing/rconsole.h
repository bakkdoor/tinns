#ifndef RCONSOLE_H
#define RCONSOLE_H

#ifdef _MSC_VER
	#pragma once
#endif

class PRConsole 
{
	private :
		typedef std::list<struct PRConClient*> ClientList;
		ClientList mClients;
		NLsocket mListener;
		
		void ProcessClient(PRConClient* Client);
		void Prompt(PRConClient *Client);
	public :
		PRConsole();
		~PRConsole();
		
		void Start();
		void Update();
};

#endif

