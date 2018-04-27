package com.ndk.myndk;

import android.app.Activity;
import android.app.Fragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.Toast;

import java.util.regex.Pattern;

/**
 * Created by j.guzikowski on 4/4/18.
 */

public class StoreActivity extends Activity {
    public static class PlaceholderFragment extends Fragment{
      // private Store mStore = new Store();
        private EditText mUIKeyEdit, mUIValueEdit;
        private Spinner mUITypeSpiner;
        private Button mUIGetButton, mUISetButton;
        private Pattern mKeyPattern;

        @Override
        public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState){
            View rootView = inflater.inflate(R.layout.fragment_layout,container,false);
            updateTitle();

            mKeyPattern = Pattern.compile("\\p{Alnum}");
            mUIKeyEdit = (EditText) rootView.findViewById(R.id.uiKeyEdit);
            mUIValueEdit = (EditText) rootView.findViewById(R.id.uiValueEdit);

            ArrayAdapter<StoreType> adapter = new ArrayAdapter<StoreType>(getActivity(),
                    R.layout.support_simple_spinner_dropdown_item,StoreType.values());

            adapter.setDropDownViewResource(
                    R.layout.support_simple_spinner_dropdown_item);

            mUITypeSpiner = (Spinner) rootView.findViewById(R.id.uiTypeSpiner);
            mUITypeSpiner.setAdapter(adapter);

            mUIGetButton = (Button) rootView.findViewById(R.id.uiGetValueButton);
            mUIGetButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    onGetValue();
                }
            });

            mUISetButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    onSetValue();
                }
            });

            return rootView;
        }

        private void onGetValue() {
            String key = mUIKeyEdit.getText().toString();
            StoreType type = (StoreType) mUITypeSpiner.getSelectedItem();

           /* if(!mKeyPattern.matcher(key).matches()){
                displayMessage("Incorect key.");
                return;
            }*/


           /* switch (type){
                case String:
                    mUIValueEdit.setText(mStore.getString(key));
                    break;
            }*/

        }

        private void onSetValue() {
            String key = mUIKeyEdit.getText().toString();
            String value = mUIValueEdit.getText().toString();

            StoreType type = (StoreType) mUITypeSpiner.getSelectedItem();

            if(!mKeyPattern.matcher(key).matches()){
                displayMessage("Incorect key.");
                return;
            }

            try {
                switch (type){
                    case String:
//                        mStore.setString(key,value);
                        break;

            }

            } catch (Exception eException){
                displayMessage("Incorect key.");
            }
            updateTitle();
        }

        private void updateTitle() {
            /*int numEntries = mStore.getCount();
            getActivity().setTitle(String.format("Store (%1$s)",numEntries));*/
        }

        private void displayMessage(String pMessage){
            Toast.makeText(getActivity(),pMessage,Toast.LENGTH_LONG).show();
        }




    }

}
