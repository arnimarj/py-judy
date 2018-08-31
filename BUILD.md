# Build instructions

How to build multi linux binaries and Mac binaries. 

## 1) Linux Build

Move to the directory containing this file and run manylinux1 build container:

```bash
docker run --rm -it -v $(pwd):/src quay.io/pypa/manylinux1_x86_64
```

From within the docker container:

```bash
cd /src

# Clean up files 
find ./judy-1.0.5/ -type f \( -name "*.o" -o -name "*.a" -o -name "*.so" \) -exec rm {} \;

find . -type f \( -name "*.o" -o -name "*.a" -o -name "*.so" \) -exec rm {} \;

for PYBIN in /opt/python/*/bin; do
  "${PYBIN}/pip" wheel . -w ./wheelhouse
done

for whl in ./wheelhouse/*.whl; do
  auditwheel repair "$whl" -w ./dist
done
```

## 2) Mac OS X Build

```bash 
# Clean up files 
find ./judy-1.0.5/ -type f \( -name "*.o" -o -name "*.a" -o -name "*.so" \) -exec rm {} \;

../<python-ver>/python setup.py bdist_wheel
../<other-ver>/python setup.py bdist_wheel
```

## 3a) Upload for testing

```bash 
twine upload --repository-url https://test.pypi.org/legacy/ dist/*
```

Test using:

```bash 
pip install https://test-files.pythonhosted.org/packages/.../judy-1.0.5-cp27-cp27mu-manylinux1_x86_64.whl
```

## 3b) Upload to production

```bash 
twine upload dist/*
```

Test using:

```bash 
pip install judy
```
