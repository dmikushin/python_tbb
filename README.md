Test to get TBB-based multithreading to work together with Python/pybind11.

## Testing

Works normally with 1 TBB thread:

```
TBB_NUM_THREADS=1 PYTHONPATH=$(pwd) python3 ../src/frontend.py
Using 1 threads
```

Hangs with any non-unit number of TBB threads:

```
TBB_NUM_THREADS=2 PYTHONPATH=$(pwd) python3 ../src/frontend.py
Using 2 threads
(hang)
```

