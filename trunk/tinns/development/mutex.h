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
	mutex.h - part of a C++ thread model using the POSIX way

	MODIFIED: 26 Sep 2005 Akiko
	REASON: - started the implementaion of a thread model
*/


#ifndef MUTEX_H
#define MUTEX_H

//this class needs pthread.h, but it should come from tinns.h

class Mutex {
	private:
		mutable pthread_mutex_t mut;
		void operator = (Mutex &mut) {
		}
		Mutex(const Mutex &mut) {
		}

	public:
		Mutex() {
			pthread_mutexattr_t attr;
			pthread_mutexattr_init(&attr);
			pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
			pthread_mutex_init(&mut, &attr);
			pthread_mutexattr_destroy(&attr);
		}

		virtual ~Mutex() {
			pthread_mutex_unlock(&mut);
			pthread_mutex_destroy(&mut);
		}

		int Lock() const {
			return(pthread_mutex_lock(&mut));
		}

		int TryLock() const {
			return(pthread_mutex_trylock(&mut));
		}

		int Unlock() const {
			return(pthread_mutex_unlock(&mut));
		}
};

#endif

