import pandas as pd
import os


def convert_txt_xlsx(working_dir):
     working_dir = "/afs/crc.nd.edu/user/j/jzhou24/scripts/vine-run-info/2023-09-11T155253/vine-logs/"
     input_file = os.path.join(working_dir, "performance")
     output_file = os.path.join(working_dir, "performance.xlsx")

     data = pd.read_csv(input_file, sep=' ')

     cols = list(data.columns)
     cols.remove('#')
     cols.append(None)
     data.columns = cols

     data.to_excel(output_file, index=False)