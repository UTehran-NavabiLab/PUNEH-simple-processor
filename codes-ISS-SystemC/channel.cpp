#include "channel.h"

void main_channel::put(sc_lv<16> data)
{
	put(data.to_int());
}

void main_channel::put(int data)
{
	//cout << "PUT 1.0" << endl;
	put_entered = true;
	put_ready.notify();
	wait(0,SC_NS);
	//cout << "PUT 1.1" << endl;
	if (!get_entered)
	{
		if (reset_occurred)
			reset_started();
		//cout << "PUT 1.2" << endl;
		wait(get_ready);
	}
	//cout << "PUT 1.3" << endl;
	bus_data = data;
	put_event.notify();

	if (reset_occurred)
		reset_started();
	//cout << "PUT 1.4" << endl;
	wait(get_event);
	put_entered = false;
	//cout << "PUT 1.5" << endl;
}

void main_channel::get(sc_lv<16>& data)
{
	//cout << "GET 2.0" << endl;
	get_entered = true;
	get_ready.notify();
	//cout << "GET 2.1" << endl;
	if (!put_entered)
	{
		if (reset_occurred)
			reset_started();
		//cout << "GET 2.2" << endl;
		wait(put_ready);
	}
	//cout << "GET 2.3" << endl;
	if (reset_occurred)
		reset_started();
	wait(put_event);
	//cout << "GET 2.4" << endl;
	data = bus_data;
	get_event.notify();
	
	get_entered = false;
	//cout << "GET 2.5" << endl;
}

void main_channel::get(int& data)
{
	get_entered = true;
	get_ready.notify();
	if (!put_entered)
	{
		if (reset_occurred)
			reset_started();
		wait(put_ready);
	}

	if (reset_occurred)
		reset_started();
	wait(put_event);

	data = bus_data.to_int();
	get_event.notify();

	get_entered = false;
}

/*
void main_channel::put(sc_lv<16> data)
{
	cout << "put here 1.0" << endl;
	wait(get_ready);
	cout << "put here 1.1" << endl;
	if (reset_occurred)
		resseting();
	bus_data = data;
	cout << "put here 1.2" << endl;
	put_event.notify();
	cout << "put here 1.3" << endl;
	if (reset_occurred)
		resseting();
	wait(get_event);
	cout << "put here 1.4" << endl;
}

void main_channel::put(int data)
{
	cout << "put here 1.0" << endl;
	wait(get_ready);
	cout << "put here 1.1" << endl;
	if (reset_occurred)
		resseting();
	bus_data = data;
	cout << "put here 1.2" << endl;
	put_event.notify();
	cout << "put here 1.3" << endl;
	if (reset_occurred)
		resseting();
	wait(get_event);
	cout << "put here 1.4" << endl;
}

void main_channel::get(sc_lv<16>& data)
{
	cout << "get here 2.0" << endl;
	wait(0, SC_NS);
	get_ready.notify();
	cout << "get here 2.1" << endl;
	wait(put_event);
	cout << "get here 2.2" << endl;
	data = bus_data;
	if (reset_occurred)
		resseting();
	get_event.notify();
	cout << "get here 2.3" << endl;

}

void main_channel::get(int& data)
{
	cout << "get here 2.0" << endl;
	wait(0, SC_NS);
	get_ready.notify();
	cout << "get here 2.1" << endl;
	wait(put_event);
	cout << "get here 2.2" << endl;
	data = bus_data.to_int();
	if (reset_occurred)
		resseting();
	get_event.notify();
	cout << "get here 2.3" << endl;
}
*/

void main_channel::get_info(operation &op, sc_lv<16> &address, burst &burst_cnt)
{
	op = bus_op;
	address = bus_address;
	burst_cnt = bus_burst_cnt;
}

void main_channel::get_info(transaction_info &info)
{
	info.op = bus_op;
	info.address = bus_address;
	info.burst_cnt = bus_burst_cnt;
}

void main_channel::wait_for_call(sc_lv<16> add_min, sc_lv<16> add_max)
{
	while (true)
	{
		wait(new_lock);
		if ((bus_address.to_int() >= add_min.to_int()) && (bus_address.to_int() <= add_max.to_int()))
			break;
		if (reset_occurred)
			break;
	}
}

void main_channel::wait_for_call(sc_lv<16> add)
{
	while (true)
	{
		wait(new_lock);
		if (bus_address == add)
			break;
		if (reset_occurred)
			break;
	}
}

void main_channel::wait_for_call()
{
	wait(new_lock);
}

void main_channel::unlock()
{
	arbitered = false;
	accesses[in_progress] = false;
	in_progress = -1;
	new_unlock.notify();
}

void main_channel::trylock(int priority, operation op, sc_lv<16> address, burst burst_cnt)
{
	bool done = false;

	if (priority >= access_cnt)
	{
		cout << "invalid priority " << priority << " out-of-range of 0 to " << access_cnt << endl;
		cout << "priority changed to " << access_cnt << " - 1" << endl;
		priority = access_cnt - 1;
	}

	accesses[priority] = true;

	while (1)
	{
		if (arbitered)
		{
			if (in_progress == priority)
				break;
			wait(new_unlock);
			if (reset_occurred)
				break;
			arbiter();
		}
		arbiter();
	}

	bus_op = op;
	bus_burst_cnt = burst_cnt;
	bus_address = address;
	wait(0, SC_NS);
	new_lock.notify();
	if (reset_occurred)
		reset_started();
}

void main_channel::trylock(int priority, transaction_info info)
{
	bool done = false;

	if (priority >= access_cnt)
	{
		cout << "invalid priority " << priority << " out-of-range of 0 to " << access_cnt << endl;
		cout << "priority changed to " << access_cnt << " - 1" << endl;
		priority = access_cnt - 1;
	}

	accesses[priority] = true;

	while (1)
	{
		if (arbitered)
		{
			if (in_progress == priority)
				break;
			wait(new_unlock);
			if (reset_occurred)
				break;
			arbiter();
		}
		arbiter();
	}

	bus_op = info.op;
	bus_burst_cnt = info.burst_cnt;
	bus_address = info.address;
	wait(0, SC_NS);
	new_lock.notify();
	if (reset_occurred)
		reset_started();
}

void main_channel::arbiter()
{
	int i;
	if (!arbitered)
	{
		for (i = 0; i < access_cnt; i++)
		{
			if (accesses[i] == true)
			{
				in_progress = i;
				arbitered = true;
				break;
			}
		}
		if (!arbitered) { throw "invalid arbiteration :D"; }
	}
	
	
}

void main_channel::resseting()
{
	bus_op = read;
	bus_burst_cnt = one;
	bus_address = 0XDEAD;
	bus_data = 0XDEAD;
	for (int i = 0; i < access_cnt;i++)
		accesses[i] = false;
	in_progress = -1;
	arbitered = false;

	put_entered = false;
	get_entered = false;
}

void main_channel::reset_started()
{
	reset_occurred = true;
	put_ready.notify();
	get_ready.notify();
	get_event.notify();
	put_event.notify();
	new_lock.notify();
	new_unlock.notify();
	resseting();
}

void main_channel::reset_stopped()
{
	reset_occurred = false;
}

