#we have to have the root dir of the repository in sys.path.
import os.path
import sys
import shutil
import subprocess

repository_root = os.path.split(os.path.split(os.path.abspath(__file__))[0])[0]
sys.path = [repository_root] + sys.path
import docgen

if __name__ == '__main__':
	print "Building documentation..."
	dest_dir = os.path.join(repository_root, 'build', 'documentation')

	node_reference = docgen.make_node_reference()
	c_api_docs=docgen.make_c_api()
	if os.path.exists(dest_dir):
		shutil.rmtree(dest_dir)

	shutil.copytree(os.path.join(repository_root, 'documentation'), dest_dir)

	with file(os.path.join(dest_dir, 'node_reference.asciidoc'), 'wb') as f:
		f.write(node_reference)
	with file(os.path.join(dest_dir, 'c_api.asciidoc'), 'wb') as f:
		f.write(c_api_docs)
	print subprocess.check_output(["asciidoc", os.path.join(dest_dir, 'libaudioverse_manual.asciidoc')], stderr= subprocess.STDOUT, shell = True)
