/*
	TinNS (TinNS is not a Neocron Server)
	Copyright (C) 2005 Linux Addicted Community
	maintainer Akiko <akiko@gmx.org>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
	02110-1301, USA.
*/



/*
	thread.h - main class of a C++ thread model using the POSIX way

	MODIFIED: 26 Sep 2005 Akiko
	REASON: - started the implementaion of a thread model
	MODIFIED: 28 Sep 2005 Akiko
	REASON: - fixed the issue with gxx 3.4.3 and gcc 4.x
*/


#ifndef THREAD_H
#define THREAD_H

//this class needs mutex.h, semaphore.h and pthread.h, but it should all come
//from tinns.h

#define INVALID_HANDLE 0

typedef void *(*pthread_fn)(void*);

template <typename Thread_T>
class Thread {
	private:
		typedef struct Instance;

	public:
		typedef Thread_T &Thread_;
		typedef const Thread_T &Thread__;
		typedef pthread_t Handle;
		typedef void (*Handler)(Thread_);

	protected:
		Thread() {
		}

		virtual void ThreadMain(Thread_) = 0;

		static void Exit() {
			pthread_exit(0);
		}

		static void TestCancel() {
			pthread_testcancel();
		}

		static Handle Self() {
			return(Handle)pthread_self();
		}

	public:
		static int Create(const Handler &Func, Thread__ Param, Handle *const &H = 0,
				  const bool &CreateDetached = false, const unsigned int &StackSize = 0,
				  const bool &CancelEnable = false, const bool &CancelAsync = false) {
			M_Create().Lock();
			pthread_attr_t attr;
			pthread_attr_init(&attr);

			if(CreateDetached)
				pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

			if(StackSize)
				pthread_attr_setstacksize(&attr, StackSize);

			Instance I(Param, 0, Func, CancelEnable, CancelAsync);

			Handle h = INVALID_HANDLE;
			int run = pthread_create((pthread_t *)&h, &attr, (pthread_fn)ThreadMainHandler, (void *)&I);

			pthread_attr_destroy(&attr);

			if(H)
				*H = h;
			if(!run)
				S_Create().Wait();

			M_Create().Unlock();

			return(errno);
		}

		int Create(Thread__ Param, Handle *const &H = 0, const bool &CreateDetached = false,
			   const unsigned int &StackSize = 0, const bool &CancelEnable = false,
			   const bool &CancelAsync = false ) const {
			M_Create().Lock();
			pthread_attr_t attr;
			pthread_attr_init(&attr);

			if(CreateDetached)
				pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

			if(StackSize)
				pthread_attr_setstacksize(&attr, StackSize);

			Instance I(Param, const_cast<Thread *>(this), 0, CancelEnable, CancelAsync);

			Handle h = INVALID_HANDLE;
			int run = pthread_create((pthread_t *)&h, &attr, (pthread_fn)ThreadMainHandler, (void *)&I);

			pthread_attr_destroy(&attr);

			if(H)
				*H = h;
			if(!run)
				S_Create().Wait();

			M_Create().Unlock();

			return(errno);
		}

		static int Join(Handle H) {
			return(pthread_join(H, 0));
		}

		static int Kill(Handle H) {
			return(pthread_cancel(H));
		}

		static int Detach(Handle H) {
			return(pthread_detach(H));
		}

	private:
		static const Mutex &M_Create() {
			static Mutex M;

			return(M);
		}

		static const Semaphore &S_Create() {
			static Semaphore S;

			return(S);
		}

		static void ThreadMainHandler(Instance *Param) {
			Instance I(*Param);
			Thread_T Data(I.Data);

			S_Create().Post();

			if(I.Flags & 1) {
				pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

				if(I.Flags & 2)
					pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
				else
					pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
			}
			else
				pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

			if(I.Owner)
				I.Owner->ThreadMain(Data);
			else
				I.pFN(Data);

			return(0);
		}

		struct Instance {
			Instance(Thread__ P, Thread<Thread_T> *const &O, const Thread<Thread_T>::Handler &pH = 0,
				 const bool &CE = false, const bool &CA = false) : Data(P), Owner(O), pFN(pH),
				 Flags(0) {
				if(CE)
					Flags |= 1;
				if(CA)
					Flags |= 2;
			}

			Thread__ Data;
			Thread<Thread_T> *Owner;
			Handler pFN;
			unsigned char Flags;
		};
};

/*
	implementation without thread parameters
*/

/*template <>
class Thread<void> {
	private:
		typedef struct Instance;

	public:
		typedef pthread_t Handle;
		typedef void (*Handler)();

	protected:
		Thread<void>() {
		}

		virtual void ThreadMain() = 0;

		static void Exit() {
			pthread_exit(0);
		}

		static void TextCancel() {
			pthread_testcancel();
		}

		static Handle Self() {
			return((Handle)pthread_self());
		}

	public:
		static int Create(const Handler &Func, Handle *const &H = 0, const bool &CreateDetached = false,
				  const unsigned int &StackSize = 0, const bool &CancelEnable = false,
				  const bool &CancelAsync = false) {
			M_Create().Locl();
			pthread_attr_t attr;
			pthread_attr_init(&attr);

			if(CreateDetached)
				pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

			if(StackSize)
				pthread_attr_setstacksize(&attr, StackSize);

			Instance I(0, Func, CancelEnable, CancelAsync);

			int run = pthread_create((pthread_t *)H, &attr, ThreadMainHandler, (void *)&I);
			pthread_attr_destroy(&attr);

			if(!run)
				S_Create().Wait();
			else if(H)
				*H = INVALID_HANDLE;

			M_Create().Unlock();

			return(errno);
		} */

#endif

