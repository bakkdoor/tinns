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
	semaphore.h - part of the C++ threading model (an very efficently way
		      of dealing with threads) - also a POSIX way

	MODIFIED: 26 Sep 2005 Akiko
	REASON: - started threading model
*/

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

//this class uses semaphore.h, but it should come from tinns.h

class Semaphore {
	private:
		sem_t sem;
	
	public:
		Semaphore(int init = 0) {
			sem_init(&sem, 0, init);
		}

		virtual ~Semaphore() {
			sem_destroy(&sem);
		}

		void Wait() const {
			sem_wait((sem_t *)&sem);
		}

		int TryWait() const {
			return(sem_trywait((sem_t *)&sem) ? errno : 0);
		}

		int Post() const {
			return(sem_post((sem_t *)&sem) ? errno : 0);
		}

		int GetValue() const {
			int val = -1;

			sem_getvalue((sem_t *)&sem, &val);

			return(val);
		}

		void Reset(int init = 0) {
			sem_destroy(&sem);
			sem_init(&sem, 0, init);
		}
};

#endif

